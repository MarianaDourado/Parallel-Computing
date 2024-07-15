#include <bits/stdc++.h>
using namespace std;

double time_in_ms(clock_t t_start, clock_t t_end){
    return ((1000.0 * t_end - 1000.0 * t_start) / CLOCKS_PER_SEC);
}

#define TRAIN_FILE "train.txt"
#define INF 1e9

int nbPoints, sizePoints;
vector<vector<double>> points;
vector<int> labels;
map<string, int> labelToIdx = {{"-1", -1}};
map<int, string> idxToLabel = {{-1, "-1"}};

void readTrainFile(){
    ifstream file(TRAIN_FILE);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo!" << std::endl;
        return;
    }

    double val;
    string label;

    file >> nbPoints >> sizePoints;
    for (int i = 0; i < nbPoints; i++){
        points.emplace_back();
        for (int j = 0; j < sizePoints; j++){
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

string knn(vector<double> pt, int k){
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

    return idxToLabel[classe];
}

int main(){
    clock_t start, end;
    readTrainFile();
    start = clock();
    string ans = knn(points[2], 5);
    end = clock();
    cout << ans << "\n";
    cout << idxToLabel[labels[2]] << "\n";
    cout << time_in_ms(start, end) << "ms\n";
    cout << points.size() << "\n";
    return 0;
}