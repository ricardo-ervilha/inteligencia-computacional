#ifndef GENETIC_H
#define GENETIC_H

#include "OPHS.h"
#include <set>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include "util.h"
#include "greedy_v2.h"

int rouletteWheel(vector<int> fitnessSolutions, int tamPop, int totalScore){
    double random_number = rand() / (float)RAND_MAX;
    
    // cout << random_number << endl;
    
    float val_acumulado = 0;
    int i;
    for(i = 0; i < tamPop; i++){
        val_acumulado += fitnessSolutions[i] / (float) totalScore ;
        if(random_number <= val_acumulado){
            return i;
        }
    }
    return i-1;
}

vector<int> findChromosome(OPHS *data, Trip** trips){
    vector<int> hotels;

    hotels.push_back(trips[0]->getStartHotel());
    for(int i = 0; i < data->getNumTrips(); i++){
        
        hotels.push_back(trips[0]->getEndHotel());
        
    } 

    return hotels;
}

void genetic_algorithm(OPHS* data, int tamPop, float percentualCrossOver, float percentualMutacao, mt19937 *gen){
    
    vector<Trip**> populacao;
    vector<int> fitnessSolutions;
    vector<vector<int>> cromossomos;

    /*Geração da População Inicial*/

    Trip **copia = makeCopySolution(data, data->getTrips()); //Trips vazias

    int totalScore = 0;

    for(int i = 0; i < tamPop; i++){

        greedy_randomized_adaptive_reactive_procedure(data, gen); //Gero solução
        populacao.push_back(makeCopySolution(data, data->getTrips())); //Salvo solução na população

        int score = getScoreTour(data, data->getTrips()); //Calculo o score do tour
        totalScore += score;
        fitnessSolutions.push_back(getScoreTour(data, data->getTrips())); //Salvo score na lista de fitness

        vector<int> chromossome = findChromosome(data, data->getTrips());
        cromossomos.push_back(chromossome);

        data->setTrips(copia);
        
        copia = makeCopySolution(data, data->getTrips());
    }

    //Imprime o score da população inicial (Geração 0)  
    // for(int i = 0; i < tamPop; i++){
    //     cout << "Score da Solução " << i << ": " << fitnessSolutions[i] << endl;
    // }

    int maxNumberOfGenerations = 10;

    // for(int i = 1; i <= percentualCrossOver * tamPop; i++){
        
    // }

} 

#endif