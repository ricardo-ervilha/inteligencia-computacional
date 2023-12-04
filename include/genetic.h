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
#include "local_search.h"


//Consertar o funcionamento do método da roleta
int rouletteWheel(vector<int> fitnessSolutions, int tamPop, int totalScore, mt19937 *gen){
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
        
        hotels.push_back(trips[i]->getEndHotel());
        
    } 

    return hotels;
}

bool comparatorNodes(Node a, Node b)
{
    return a.score < b.score;
}

// bool verifiesFeasibility(int tripPivot, vector<Trip**> populacao, int indexParent1, int indexParent2, OPHS* data){}

void applyCrossOverI(OPHS* data, int indexParent1, int indexParent2, vector<Trip**> populacao, vector<vector<int>> cromossomos, mt19937 *gen){

    // [a b c d] -> supondo essa combinação de hoteis
    // (Última trip de fora) permitir sortear somente as trips [a b] [b c], visto que sortear a última [c d] só faria trocar o valor de d. Mas, valor de d no nosso problema é fixo.  
    
    //One of the trips (trip d) of the first parent, P1, is randomly selected. This trip is called the pivot trip
    int tripPivot = intRandom(0, data->getNumTrips()-2, gen);

    cout << "Index do pai 1: " << indexParent1 << endl;
    cout << "Index do pai 2: " << indexParent2 << endl;

    //The algorithm then checks if it is feasible for this pivot trip to start from the initial hotel of the selected trip in the first parent solution and end in the final hotel of the selected trip in the second parent solution. This check, can be done efficiently by using the matrix of pairs of hotels.If the condition is false, another trip is selected randomly.
    
    //data->getMatrix()[tripPivot][populacao[indexParent1][tripPivot]->getStartHotel()][populacao[indexParent2][tripPivot]->getEndHotel()] == false
    
    while(data->getMatrix()[tripPivot][populacao[indexParent1][tripPivot]->getStartHotel()][populacao[indexParent2][tripPivot]->getEndHotel()] == false){

        tripPivot = intRandom(0, data->getNumTrips()-2, gen);
    }

    cout << "=> Indice da trip pivot: " << tripPivot << endl;
    
    //If the condition in (b) is true, the new sequence of hotels is formed by taking the hotel sequence of the first part of the first parent up to the initial hotel of the pivot trip and combining it with the hotel sequence from the final hotel of the pivot trip in the second parent, to the final depot.
    vector<int> new_chromossome;

    //Primeira parte vindo do P1
    new_chromossome.push_back(0);
    
    for(int i = 0; i < tripPivot; i++){    
        new_chromossome.push_back(populacao[indexParent1][i]->getEndHotel());
    }
    
    //Segunda parte vindo do P2
    for(int i = tripPivot; i < data->getNumTrips(); i++){
        new_chromossome.push_back(cromossomos[indexParent2][i+1]);
    }
    
    // cout << "[ ";
    // for(int j = 0; j < cromossomos[indexParent1].size(); j++){
    //     cout << cromossomos[indexParent1][j] << " ";
    // }
    // cout << "]  ";
    // cout << "Cromossomo Pai 1\n";

    // cout << "[ ";
    // for(int j = 0; j < cromossomos[indexParent2].size(); j++){
    //     cout << cromossomos[indexParent2][j] << " ";
    // }
    // cout << "]  ";
    // cout << "Cromossomo Pai 2\n";

    // cout << "[ ";
    // for(int j = 0; j < new_chromossome.size(); j++){
    //     cout << new_chromossome[j] << " ";
    // }
    // cout << "]  ";
    // cout << "Cromossomo Filho\n";


    //Inicializa os hoteis
    int cont = 0;
    while(cont < data->getNumTrips()){
        data->getTrip(cont)->setStartHotel(new_chromossome[cont]);
        data->getTrip(cont)->setEndHotel(new_chromossome[cont+1]);
        cont++;
    }

    set<int> visiteds;

    //Enchendo as trips antes da trip pivo pois podem permanecer iguais
    for(int j = 0; j < tripPivot; j++){
        for(int i = 0; i < populacao[indexParent1][j]->getNodes().size(); i++){
            data->getTrip(j)->add(populacao[indexParent1][j]->getNode(i));
            visiteds.insert(populacao[indexParent1][j]->getNode(i).id); //adiciona os nos visitados das trips antes da pivô
        }
        data->getTrip(j)->setCurrentLength(calcTripLength(data, data->getTrip(j)));
    }

    //Enchendo a trip pivo com os POI's derivados do primeiro pai
    for(int i = 0; i < populacao[indexParent1][tripPivot]->getNodes().size(); i++){
        data->getTrip(tripPivot)->add(populacao[indexParent1][tripPivot]->getNode(i));
    }

    data->getTrip(tripPivot)->setCurrentLength(calcTripLength(data, data->getTrip(tripPivot)));

    // data->printDadosOPHS();

    vector<Node> *nos = data->getTrip(tripPivot)->getNodesPointer();

    while(data->getTrip(tripPivot)->getCurrentLength() > data->getTrip(tripPivot)->getTd()){
        //Pego o nó de menor score
        int index = intRandom(0, nos->size() - 1, gen);

        cout << "Index: " << index << endl;

        nos->erase(nos->begin() + index);

        //Atualizo o trip length
        data->getTrip(tripPivot)->setCurrentLength(calcTripLength(data, data->getTrip(tripPivot)));
    }

    for(int i = 0; i < data->getTrip(tripPivot)->getNodes().size(); i++){
            visiteds.insert(data->getTrip(tripPivot)->getNode(i).id); //adiciona os nós visitados trip pivo
    }

    //Completa a parte final do tour.  
    Trip** sol = constructive_algorithm_modified(data, gen, visiteds, tripPivot+1, 0);

    // data->printDadosOPHS();
}

void genetic_algorithm(OPHS* data, int tamPop, float percentualCrossOver, float percentualMutacao, float bestSel_R, mt19937 *gen){

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

    cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    // Imprime o score da população inicial (Geração 0)  
    for(int i = 0; i < tamPop; i++){
        cout << "[ ";
        for(int j = 0; j < cromossomos[i].size(); j++){
            cout << cromossomos[i][j] << " ";
        }
        cout << "]  ";
        cout << "Score da Solução " << i << ": " << fitnessSolutions[i] << endl;
    }
    cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    // int maxNumberOfGenerations = 10;

    /* Aplicação do CrossOver I*/
    for (int k = 0; k < 8; k++)
    {

        for (int i = 1; i <= (int) 2 * percentualCrossOver * tamPop; i++)
        {
            int indexParent1 = intRandom(0, tamPop - 1, gen);
            int indexParent2 = intRandom(0, tamPop - 1, gen);
            while (indexParent1 == indexParent2)
            {
                cout << "Caiu no while da roleta!\n";
                indexParent2 = intRandom(0, tamPop - 1, gen);
            }

            if (i % 2 != 0)
                applyCrossOverI(data, indexParent1, indexParent2, populacao, cromossomos, gen); // Aplica crossover com tripPivot no p1
            else
                applyCrossOverI(data, indexParent2, indexParent1, populacao, cromossomos, gen); // Aplica crossover com tripPivot no p2

            populacao.push_back(makeCopySolution(data, data->getTrips())); // Salvo solução na população

            int score = getScoreTour(data, data->getTrips()); // Calculo o score do tour
            totalScore += score;
            fitnessSolutions.push_back(getScoreTour(data, data->getTrips())); // Salvo score na lista de fitness

            vector<int> chromossome = findChromosome(data, data->getTrips());
            cromossomos.push_back(chromossome);

            data->setTrips(copia);
            copia = makeCopySolution(data, data->getTrips());
        }

        cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
        // Imprime o score depois dos novos crossovers
        for (int i = 0; i < populacao.size(); i++)
        {
            cout << "[ ";
            for (int j = 0; j < cromossomos[i].size(); j++)
            {
                cout << cromossomos[i][j] << " ";
            }
            cout << "]  ";
            cout << "Score da Solução " << i << ": " << fitnessSolutions[i] << endl;
        }
        cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";

        /*Gerenciamento da População*/
        vector<Trip **> novaPopulacao;
        vector<int> novosFitnessSolutions;
        vector<vector<int>> novosCromossomos;

        vector<int> indicesOrdenados;
        for (int j = 0; j < fitnessSolutions.size(); j++)
        {
            indicesOrdenados.push_back(j);
        }
        std::sort(indicesOrdenados.begin(), indicesOrdenados.end(),
                  [&fitnessSolutions](int a, int b)
                  {
                      return fitnessSolutions[a] > fitnessSolutions[b];
                  });

        int cont = 0;
        for (; cont < tamPop; cont++)
        {
            novaPopulacao.push_back(populacao[indicesOrdenados[cont]]);
            novosFitnessSolutions.push_back(fitnessSolutions[indicesOrdenados[cont]]);
            novosCromossomos.push_back(cromossomos[indicesOrdenados[cont]]);
        }

        populacao = novaPopulacao;
        fitnessSolutions = novosFitnessSolutions;
        cromossomos = novosCromossomos;

        cout << "-+-+-+-+-+-+-Gerenciamento da População-+-+-+-+-+-+-+-+-+-+\n";
        // Imprime o score do pool final nessa geração.
        for (int i = 0; i < tamPop; i++)
        {
            cout << "[ ";
            for (int j = 0; j < cromossomos[i].size(); j++)
            {
                cout << cromossomos[i][j] << " ";
            }
            cout << "]  ";
            cout << "Score da Solução " << i << ": " << fitnessSolutions[i] << endl;
        }
        cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
    }
    
} 

#endif