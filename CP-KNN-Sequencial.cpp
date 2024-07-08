#include <iostream>
#include <assert.h>
#include <cstring>
#include<bits/stdc++.h>
// descomentar p paralelo
//#include "utilshost.cuh"
using namespace std;
typedef long long ll;

double time_in_ms(clock_t t_start, clock_t t_end);

// Total de imagens
const int TOTAL_IMGS = 15000;//?
// Tamanho do vetor de embedding
const int EMB_SIZE = 2048;

// le os embedding de um arquivo que o dayllon vai gerar, pode deixar com a MARI
void readEmbeddings(){

}

//distancias de cada emb até a query
int distancesToQuery[TOTAL_IMGS];

// lista com embedding de cada imagem
int listEmbeddings[TOTAL_IMGS][EMB_SIZE];

// a gente vai pegar a query de um arquivo também
int queryEmbedding[EMB_SIZE];

void computeDistanceToQuery(){
    int id = 0;
    // para cada vetor na lista de embeddings
    for(auto nbEmbedding : listEmbeddings){
        // para cada elemento no vetor de embeddings
        for(int i=0; i<EMB_SIZE; i++){
            // distância euclidiana
            distancesToQuery[id] = distancesToQuery[id] + ((queryEmbedding[i] - nbEmbedding[i])*(queryEmbedding[i] - nbEmbedding[i]));
        }
        id++;
    }
}

void sortDistances(){
    for (int i = 0; i < TOTAL_IMGS; i++){
        // sort algorithm, pode ser outro
        if(distancesToQuery[i] > distancesToQuery[i+1]){
            //swap
            //...
        }
    }
}

int KNN(int K){
    computeDistanceToQuery();
    sortDistances();
    // verificar os K vizinhos mais proximos
    for (int i = 0; i < K; i++)
    {
        /* code */
    }
    

    // pega a label que mais se repete entre os K
    // retorna
    return 0;

}


int main(){
    clock_t inicio, fim;
    memset(distancesToQuery, 0, sizeof(distancesToQuery));

    int K = 3;

    // a label de verdade da imagem input
    int y = -1;

    inicio = clock();
    
    int ans =  KNN(K);
    
    fim = clock();

    cout << "K = " << K << "\n";
    cout << "A predição diz que a imagem é da classe: " << ans << "\n";
    cout << "Na verdade ela é da classe: " << y << "\n";
    cout << "O tempo necessário para fazer essa inferência foi de " << time_in_ms(inicio, fim) << " milissegundos\n";

    return 0;

}

double time_in_ms(clock_t t_start, clock_t t_end)
{
    return ((1000.0 * t_end - 1000.0 * t_start) / CLOCKS_PER_SEC);
}