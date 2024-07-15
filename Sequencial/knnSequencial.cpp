#include <bits/stdc++.h>
using namespace std;

// tempo que passou em ms
double time_in_ms(clock_t t_start, clock_t t_end){
    return ((1000.0 * t_end - 1000.0 * t_start) / CLOCKS_PER_SEC);
}

// definição dos treinos, testes e INFinito
#define TRAIN_FILE "../train.txt"
#define TEST_FILE "../validation.txt"
#define INF 1e9

//transforma label de string em int "0001" -> 1
map<string, int> labelToIdx = {{"-1", -1}};
// faz o contrário
map<int, string> idxToLabel = {{-1, "-1"}};

// nPoints = número de pontos (dados); sizePoint = tamanho do embedding (2048 é o padrão)
int nbPoints, sizePoint;
// matriz dos pontos
vector<vector<double>> points;
// labels
vector<int> labels;

int nbPointsTest, sizePointTest;
vector<vector<double>> pointsTest;
vector<int> labelsTest;

void readFile(string pathFile, int& nbPoints, int& sizePoint, vector<vector<double>>& points, vector<int>& labels){
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
        // verifica se esse label existe
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
    //euclidean sem raiz, pq n precisa , osprecisa saber se está mais perto
    for (int i = 0; i < pt1.size(); i++){
        dist += (pt1[i] - pt2[i]) * (pt1[i] - pt2[i]);
    }
    return dist;
}

// pt = query
int knn(vector<double> pt, int k){
    // distância começa INFinita 
    vector<double> kDists(k, INF);
    // não há caras próximos
    vector<int> kLabels(k, -1);
    double dist;
    int pos;

    // ando por todos os treinos
    for (int i = 0; i < nbPoints; i++){
        dist = calcDist(pt, points[i]);
        // posição que vai entrar no vetor ordeano (pos-iésimo menor cara visto até o momento)
        pos = -1;
        // meio que da um shift >>> 
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
        // se for maior do que o último vizinho mais perto (dos K), não quero ele
        if (pos != -1){
            kDists[pos] = dist;
            kLabels[pos] = labels[i];
        }
    }

    // quantas vezes cada label aparece
    unordered_map<int, int> nbOfLabels;
    for (int i = 0; i < k; i++){
        // n entra aqui se o K > pontos
        if (kLabels[i] != -1){
            nbOfLabels[kLabels[i]]++;
        }
    }

    // qual label repete mais?
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
    readFile(TRAIN_FILE, nbPoints, sizePoint, points, labels);
    readFile(TEST_FILE, nbPointsTest, sizePointTest, pointsTest, labelsTest);

    // acontece se passar o path errado
    if (sizePoint != sizePointTest){
        cout << "Train and test points does not have the same dimensions.\n";
        return 0;
    }

    // inicia o clock
    start = clock();
    // faz a inferência (query, K)
    int ans = knn(pointsTest[42], K);
    // encerro o clock
    end = clock();
    // nome do label da classe inferida
    cout << idxToLabel[ans] << "\n";
    // classe real  
    cout << idxToLabel[labelsTest[42]] << "\n";
    // quanto tempo demorou
    cout << time_in_ms(start, end) << "ms\n";
    return 0;
}
