#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#include "util.h"
#include <cmath>
#include <random>
#include <set>

using namespace std;

Trip **makeCopySolution(OPHS *data, Trip **initialSolution)
{
    Trip **copyTrips = new Trip *[data->getNumTrips()];

    // inicializar as trips
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        copyTrips[i] = new Trip(data->getTrips()[i]->getTd()); // Td   = trip max length for each trip d
        copyTrips[i]->setStartHotel(initialSolution[i]->getStartHotel());
        copyTrips[i]->setEndHotel(initialSolution[i]->getEndHotel());
        copyTrips[i]->updateCurrentLength(initialSolution[i]->getCurrentLength());
        copyTrips[i]->setNodes(initialSolution[i]->getNodes());
    }

    return copyTrips;
}
set<int> getIdsInSolution(OPHS *data, Trip **solution)
{
    set<int> idsInSolution;
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        for (const Node &node : solution[i]->getNodes())
        {
            idsInSolution.insert(node.id);
        }
    }

    return idsInSolution;
}

bool isTripFeasible(OPHS *data, Trip *trip)
{
    float lengthTrip = calcTripLength(data, trip);
    cout << "1. Tamanho máximo da trip: " << trip->getTd();
    cout << " - Tamanho da trip: " << lengthTrip << endl;

    return lengthTrip <= trip->getTd();
}

bool isTripFeasible(OPHS *data, Trip *trip, vector<Node> nos)
{
    float lengthTrip = calcTripLength(data, trip->getStartHotel(), trip->getEndHotel(), nos);
    cout << "2. Tamanho máximo da trip: " << trip->getTd();
    cout << " - Tamanho da trip: " << lengthTrip << endl;

    return lengthTrip <= trip->getTd();
}

// Arrumar um jeito de não gerar soluções repetidas
Trip **generateRandomNeighbor(OPHS *data, Trip **solution, mt19937 *gen)
{
    bool feasibleRandomSolution = false;

    int numVertices = data->getNumExtraHotels() + data->getNumVertices(); // Numero de vertices do problema
    set<int> idsInSolution = getIdsInSolution(data, solution);
    set<int> idsTested; // index dos nos que ja foram testados

    while (!feasibleRandomSolution)
    {
        int indexRandomTrip = intRandom(0, data->getNumTrips() - 1, gen); // index da trip a ser modificada
        // pega um vertice aleatorio, desconsiderando os index dos hoteis e que não esteja na solução
        int idRandomVertexAdd = intRandom(data->getNumExtraHotels() + 2, numVertices - 1, gen); // id do no que vai ser colocado

        vector<Node> copiaNodes = solution[indexRandomTrip]->getNodes();
        vector<Node> nodesTrip = solution[indexRandomTrip]->getNodes();

        // pega um no aleatório da trip [indexRandomTrip] - nodesTrip não tem os hoteis
        int indexRandomNodeRemover = intRandom(0, nodesTrip.size() - 1, gen); // index do no que vai ser removido

        while (idsInSolution.find(idRandomVertexAdd) != idsInSolution.end())
        {
            idRandomVertexAdd = intRandom(data->getNumExtraHotels() + 2, numVertices - 1, gen);
        }

        Node randomVertex = data->getVertex(idRandomVertexAdd);
        nodesTrip[indexRandomNodeRemover] = randomVertex; // Substituir por randomVertex

        float lengthTrip = calcTripLength(data, solution[indexRandomTrip]->getStartHotel(), solution[indexRandomTrip]->getEndHotel(), nodesTrip);
        if (lengthTrip <= solution[indexRandomTrip]->getTd())
        {
            // printTrips(data, solution);
            // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;

            solution[indexRandomTrip]->setNodes(nodesTrip);
            solution[indexRandomTrip]->setCurrentLength(lengthTrip);
            feasibleRandomSolution = true;

            // cout << "Gerou solução viavel" << endl;
            // cout << "index trip a ser modificada: " << indexRandomTrip << endl;
            // cout << "index no removido: " << indexRandomNodeRemover << endl;
            // cout << "id do no add: " << idRandomVertexAdd << endl;
            // printTrips(data, solution);
            // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;
        }

        // cout <<"indexRandomTrip: "<<indexRandomTrip<<" idRandomVertexAdd: "<<idRandomVertexAdd<< " indexRandomNodeRemover: "<<indexRandomNodeRemover<<endl;
    }
    return solution;
}

float updateTemperature(float T)
{
    return 0.99 * T;
}
/// @brief Gera uma solução vizinha de uma solução inicial com SA
/// @param data
/// @param initialSolution solução inicial
/// @param iterations número de iterações por temperatura
/// @param T Temperatura inicial
/// @param Tmin Temperatura mínima
/// @return Trip
Trip **simulatedAnnealing(OPHS *data, Trip **initialSolution, int iterations, float T, float Tmin, mt19937 gen)
{
    Trip **bestSolution = initialSolution;

    int iteracoesT = 0;
    while (T > Tmin)
    {
        int iter = 0;
        while (iter < iterations)
        {
            Trip **copySolution = makeCopySolution(data, initialSolution);
            Trip **neighborSolution = generateRandomNeighbor(data, copySolution, &gen);

            float neighborSolutionScore = getScoreTour(data, neighborSolution);
            float initialSolutionScore = getScoreTour(data, initialSolution);

            float deltaE = neighborSolutionScore - initialSolutionScore;

            if (deltaE > 0)
            {
                initialSolution = neighborSolution;

                // posso gerar uma solução (neighborSolution) melhor que a solução base (initialSolution), deltaE>0,
                // mas que não é melhor que a melhor solução obtida  até agora (bestSolution)
                float bestTotalScore = getScoreTour(data, bestSolution);
                if (neighborSolutionScore > bestTotalScore)
                {
                    bestSolution = neighborSolution;
                }
            }
            else
            {
                float p = exp(-deltaE / T);
                float x = doubleRandom(0.0, 1.0, &gen);
                // cout << "Temperatura T: " << T << "\t|";
                // cout << " deltaE: " << deltaE << "\t|";
                // cout << " probabilidade: " << p << "\t\t|";
                // cout << " Valor aleatorio: " << x << endl;
                if (x < p)
                {
                    initialSolution = neighborSolution;
                }
                else
                {
                    // deletar solução anterior
                }
            }

            // cout << "Iteração: " << iter << " Qualida da best: " << getScoreTour(data, bestSolution)<<endl;
            iter++;
        }
        iteracoesT++;
        T = updateTemperature(T);
        if (iteracoesT % 100 == 0)
            cout << "IteracaoTemperatura: " << iteracoesT << " Temperatura: " << T << " Qualida da best: " << getScoreTour(data, bestSolution) << endl;
    }
    return bestSolution;
}

#endif