#include <bits/stdc++.h>
#include <cuda_runtime_api.h>
#include <cuda.h>

#include "utilsGpu.cuh"
#include "utilsHost.hpp"

using namespace std;

#define TRAIN_FILE "../train.txt"
#define TEST_FILE "../validation.txt"
#define INF 1e9

void readTrainFile(string path, int& nbPoints, int& sizePoint, double*& points, int*& labels, map<string, int>& labelToIdx, map<int, string>& idxToLabel){
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo!" << std::endl;
        return;
    }

    double val;
    string label;

    file >> nbPoints >> sizePoint;

    points = (double*) allocHostMemory(nbPoints * sizePoint * sizeof(double));
    labels = (int*) allocHostMemory(nbPoints * sizeof(int));

    for (int i = 0; i < nbPoints; i++){
        for (int j = 0; j < sizePoint; j++){
            file >> val;
            points[i * sizePoint + j] = val;
        }
        file >> label;
        if (labelToIdx.count(label) == 0){
            idxToLabel[labelToIdx.size()] = label;
            labelToIdx[label] = labelToIdx.size();
        }
        labels[i] = labelToIdx[label];
    }

    file.close();
}

pair<double*, int*> setPointsGpu(int nbPoints, int sizePoint, double* points, int* labels){
    double* device_points = (double*) GPU_allocateMemory(nbPoints * sizePoint * sizeof(double));
    int* device_labels = (int*) GPU_allocateMemory(nbPoints * sizeof(int));

    GPU_setMemory(points, device_points, nbPoints * sizePoint * sizeof(double));
    GPU_setMemory(labels, device_labels, nbPoints * sizeof(int));

    return {device_points, device_labels};
}

__device__ double calcDist(double* pt1, double* pt2, int size){
    double dist = 0;
    for (int i = 0; i < size; i++){
        dist += (pt1[i] - pt2[i]) * (pt1[i] - pt2[i]);
    }
    return dist;
}

__global__ void KERNEL_compute_distances(double *pt, int K, int nbPoints, int sizePoint, double* kDists, int* kLabels, double* points, int* labels){
    int threadId = blockIdx.x * blockDim.x + threadIdx.x;

    if (threadId >= nbPoints)
        return;

    int loopStride = gridDim.x * blockDim.x;
    int start = threadId * K;

    for (int i = 0; i < K; i++){
        kDists[start + i] = INF;
        kLabels[start + i] = -1;
    }

    double dist;
    int pos;
    for (int i = threadId; i < nbPoints; i += loopStride){
        dist = calcDist(pt, &points[i * sizePoint], sizePoint);
        pos = -1;
        for (int j = K-1; j >= 0; j--){
            if (dist < kDists[start + j]){
                pos = j;
                if (j > 0){
                    kDists[start + j] = kDists[start + j - 1];
                    kLabels[start + j] = kLabels[start + j - 1];
                }
            }
            else{
                break;
            }
        }
        if (pos != -1){
            kDists[start + pos] = dist;
            kLabels[start + pos] = labels[i];
        }
        //for (int j = 0; j < K; j++){
        //    printf("%d %d -> %lf : %d\n", threadId, j, kDists[start + j], kLabels[start + j]);
        //}
    }
}

int applyKnn(double* pt, int K, int nbPoints, int sizePoint, double* device_points, int* device_labels, int nbThreadsPerBlock, int nbBlocks){
    int classe = -1;
    int nbThreads = nbThreadsPerBlock * nbBlocks;

    double* device_pt = (double*) GPU_allocateMemory(sizePoint * sizeof(double));
    GPU_setMemory(pt, device_pt, sizePoint * sizeof(double));

    double* device_kDists = (double*) GPU_allocateMemory(nbThreads * K * sizeof(double));
    int* device_kLabels = (int*) GPU_allocateMemory(nbThreads * K * sizeof(int));

    KERNEL_compute_distances<<<nbBlocks, nbThreadsPerBlock>>>(device_pt, K, nbPoints, sizePoint, device_kDists, device_kLabels, device_points, device_labels);
    handleError(cudaStreamSynchronize(0));

    double* kDists = (double*) allocHostMemory(nbThreads * K * sizeof(double));
    GPU_getMemory(kDists, device_kDists, nbThreads * K * sizeof(double));

    int* kLabels = (int*) allocHostMemory(nbThreads * K * sizeof(int));
    GPU_getMemory(kLabels, device_kLabels, nbThreads * K * sizeof(int));

    vector<pair<double, int>> distsAndLabels;
    unordered_map<int, int> nbOfLabels;
    for (int i = 0; i < nbThreads * K; i++){
        distsAndLabels.push_back({kDists[i], kLabels[i]});
    }
    sort(distsAndLabels.begin(), distsAndLabels.end());
    for (int i = 0; i < K; i++){
        if (distsAndLabels[i].second != -1){
            nbOfLabels[distsAndLabels[i].second]++;
        }
    }

    for (auto [label, nb] : nbOfLabels){
        if (nbOfLabels[classe] < nb){
            classe = label;
        }
    }

    freeHostMemory(kDists);
    freeHostMemory(kLabels);    
    GPU_freeMemory(device_pt);
    GPU_freeMemory(device_kDists);
    GPU_freeMemory(device_kLabels);

    return classe;
}


int main(int argc, char *argv[])
{
    /* --------------- Obtendo o valor de K -------------- */
    if (argc != 4) {
        cout << "The number of parameters must be 3.";
        return 0;
    }
    int K = std::atoi(argv[1]);
    int nbThreadsPerBlock = atoi(argv[2]);
    int nbBlocks = atoi(argv[3]);

    /* --------------- Definindo algumas variáveis-------------- */
    clock_t start, end, startMemo, endMemo;
    map<string, int> labelToIdx = {{"-1", -1}};
    map<int, string> idxToLabel = {{-1, "-1"}};

    /* --------------- Lendo dados de treino -------------- */
    int nbPoints, sizePoint;
    double* points;
    int* labels;

    readTrainFile(TRAIN_FILE, nbPoints, sizePoint, points, labels, labelToIdx, idxToLabel);


    /* --------------- Lendo dados de teste -------------- */
    int nbPointsTest, sizePointTest;
    double *pointsTest;
    int *labelsTest;

    readTrainFile(TEST_FILE, nbPointsTest, sizePointTest, pointsTest, labelsTest, labelToIdx, idxToLabel);

    if (sizePoint != sizePointTest){
        cout << "Train and test points does not have the same dimensions.\n";
        return 0;
    }

    /* --------------- Definindo os pontos na GPU -------------- */
    startMemo = clock();
    auto [device_points, device_labels] = setPointsGpu(nbPoints, sizePoint, points, labels);
    endMemo = clock();

    /* --------------- Testando -------------- */
    double totalTime = 0, currTime;
    for (int i = 0; i < nbPointsTest; i++){
        //cout << "Test " << i << "\n";

        /* --------------- Escolhendo um ponto para testar -------------- */
        double *pt = &pointsTest[i * sizePointTest];
        int label = labelsTest[i];

        ///* --------------- Fazendo a inferência do ponto -------------- */
        start = clock();
        int inferedLabel = applyKnn(pt, K, nbPoints, sizePoint, device_points, device_labels, nbThreadsPerBlock, nbBlocks);
        end = clock();
        cout << "INFERED LABEL: " << idxToLabel[inferedLabel] << "\n";
        cout << "CORRECT CLASS: " << idxToLabel[label] << "\n";
        currTime = time_in_ms(start, end);
        cout << "TIME: " << currTime << "\n";
        totalTime += currTime;
    }

    cout <<"TOTAL TIME: " << totalTime << "\n";
    cout << "TIME FOR MEMORY: " << time_in_ms(startMemo, endMemo) << "\n";

    freeHostMemory(points);
    freeHostMemory(labels);
    GPU_freeMemory(device_points);
    GPU_freeMemory(device_labels);
    return 0;
}