#include <bits/stdc++.h>
using namespace std;

double time_in_ms(clock_t t_start, clock_t t_end){
    return ((1000.0 * t_end - 1000.0 * t_start) / CLOCKS_PER_SEC);
}

#define TRAIN_FILE "../../train.txt"
#define TEST_FILE "../../validation.txt"
#define INF 1e9

map<string, int> labelToIdx = {{"-1", -1}};
map<int, string> idxToLabel = {{-1, "-1"}};

int nbPoints, sizePoint;
vector<vector<double>> points;
vector<int> labels;

int nbPointsTest, sizePointTest;
vector<vector<double>> pointsTest;
vector<int> labelsTest;

void readTrainFile(string pathFile, int& nbPoints, int& sizePoint, vector<vector<double>>& points, vector<int>& labels){
    ifstream file(pathFile);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo!" << std::endl;
        return;
    }

    double val;
    string label;

    file >> nbPoints >> sizePoint;
    for (int i = 0; i < nbPoints; i++){
        points.emplace_back();
        for (int j = 0; j < sizePoint; j++){
            file >> val;
            points.back().push_back(val);
        }
        file >> label;
        if (labelToIdx.count(label) == 0){
            idxToLabel[labelToIdx.size()] = label;
            labelToIdx[label] = labelToIdx.size();
        }
        labels.push_back(labelToIdx[label]);
    }

    file.close();
}

double calcDist(vector<double> &pt1, vector<double>& pt2){  
    double dist = 0;
    for (int i = 0; i < pt1.size(); i++){
        dist += (pt1[i] - pt2[i]) * (pt1[i] - pt2[i]);
    }
    return dist;
}

int knn(vector<double> pt, int k){
    vector<double> kDists(k, INF);
    vector<int> kLabels(k, -1);
    double dist;
    int pos;
    for (int i = 0; i < nbPoints; i++){
        dist = calcDist(pt, points[i]);
        pos = -1;
        for (int j = k-1; j >= 0; j--){
            if (dist < kDists[j]){
                pos = j;
                if (j > 0){
                    kDists[j] = kDists[j-1];
                    kLabels[j] = kLabels[j-1];
                }
            }
            else{
                break;
            }
        }
        if (pos != -1){
            kDists[pos] = dist;
            kLabels[pos] = labels[i];
        }
    }

    unordered_map<int, int> nbOfLabels;
    for (int i = 0; i < k; i++){
        if (kLabels[i] != -1){
            nbOfLabels[kLabels[i]]++;
        }
    }

    int classe = -1;
    for (auto [label, nb] : nbOfLabels){
        if (nbOfLabels[classe] < nb){
            classe = label;
        }
    }

    return classe;
}

int main(int argc, char *argv[]){
    /* --------------- Obtendo o valor de K -------------- */
    if (argc != 2){
        cout << "The number of parameters must be 1.";
        return 0;
    }
    int K = std::atoi(argv[1]);

    clock_t start, end;
    readTrainFile(TRAIN_FILE, nbPoints, sizePoint, points, labels);
    readTrainFile(TEST_FILE, nbPointsTest, sizePointTest, pointsTest, labelsTest);

    if (sizePoint != sizePointTest){
        cout << "Train and test points does not have the same dimensions.\n";
        return 0;
    }

    start = clock();
    int ans = knn(pointsTest[42], K);
    end = clock();
    //cout << idxToLabel[ans] << "\n";
    //cout << idxToLabel[labelsTest[42]] << "\n";
    cout << time_in_ms(start, end);
    return 0;
}
