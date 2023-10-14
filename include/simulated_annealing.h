#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#include "util.h"
#include <cmath>
#include <random>
#include <set>

using namespace std;

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

vector<int> getIdsNotInSolution(OPHS *data, set<int> idsInSolution)
{
    vector<int> idsNotInSolution;
    for (int i = data->getNumExtraHotels() + 2; i < data->getNumExtraHotels() + data->getNumVertices(); i++)
    {
        if (idsInSolution.find(data->getVertex(i).id) == idsInSolution.end())
        {
            idsNotInSolution.push_back(data->getVertex(i).id);
        }
    }

    return idsNotInSolution;
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

/*
    sortear uma trip e um index dessa trip
    verificar qual o melhor no para adicionar nessa posição
*/

Trip **generateRandomNeighbor(OPHS *data, Trip **solution, mt19937 *gen)
{
    bool feasibleRandomSolution = false;

    int numVertices = data->getNumExtraHotels() + data->getNumVertices(); // Numero de vertices do problema
    set<int> idsInSolution = getIdsInSolution(data, solution);
    vector<int> idsNotInSolution = getIdsNotInSolution(data, idsInSolution);
    set<tuple<int, int, int>> candidatosGerados; // tupla com <indexTrip, indexNoRemover, idRandomVertexAdd>

    while (!feasibleRandomSolution)
    {
        set<int> idsTested;                                               // ids dos nos que ja foram testados
        int indexRandomTrip = intRandom(0, data->getNumTrips() - 1, gen); // index da trip a ser modificada

        // pega um vertice aleatorio, desconsiderando os index dos hoteis e que não esteja na solução
        // int idRandomVertexAdd = intRandom(data->getNumExtraHotels() + 2, numVertices - 1, gen); // id do no que vai ser colocado
        int indexRandomVertexAdd = intRandom(0, idsNotInSolution.size() - 1, gen); // index no idsNotInSolution do no que vai ser colocado

        vector<Node> nodesTrip = solution[indexRandomTrip]->getNodes(); // nos da trip aleatoria

        // pega um no aleatório da trip [indexRandomTrip] - nodesTrip não tem os hoteis
        int indexRandomNodeRemover = intRandom(0, nodesTrip.size() - 1, gen); // index do no que vai ser removido

        tuple<int, int, int> candidato = make_tuple(indexRandomTrip, indexRandomNodeRemover, idsNotInSolution[indexRandomVertexAdd]);

        // fica no loop enquanto não gerar um vertex valido
        bool achouValido = true;
        // verifica se o no ja esta na solução ou se ja foi avaliado
        // diferente de end, significa que ele foi encontrado
        while ((candidatosGerados.find(candidato) != candidatosGerados.end()))
        {
            idsTested.insert(idsNotInSolution[indexRandomVertexAdd]);
            if (idsTested.size() == idsNotInSolution.size()) // testou todos os vertices e não achou nenhum viavel
            {
                achouValido = false;
                break;
            }
            indexRandomVertexAdd = intRandom(0, idsNotInSolution.size() - 1, gen); // index no idsNotInSolution do no que vai ser colocado
            candidato = make_tuple(indexRandomTrip, indexRandomNodeRemover, idsNotInSolution[indexRandomVertexAdd]);
        }

        // se não achou indexRandomVertexAdd valido, vai pra proxima iteração sortear outra trip e outro no pra remover
        if (!achouValido)
        {
            continue;
        }

        auto result = candidatosGerados.insert(candidato); // candidatos validos que ja foram avaliados

        Node randomVertex = data->getVertex(idsNotInSolution[indexRandomVertexAdd]);
        nodesTrip[indexRandomNodeRemover] = randomVertex; // Substituir por randomVertex

        float lengthTrip = calcTripLength(data, solution[indexRandomTrip]->getStartHotel(), solution[indexRandomTrip]->getEndHotel(), nodesTrip);
        // cout << "-------------vizinho------------------" << endl;
        // printIdsTrips(data, solution);
        if (lengthTrip <= solution[indexRandomTrip]->getTd())
        {
            // cout << "nós trip: " << endl;
            // for (int k = 0; k < nodesTrip.size(); k++)
            // {
            //     cout << nodesTrip[k].id << " ";
            // }
            // cout << endl;
            // printTrips(data, solution);
            // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;

            solution[indexRandomTrip]->setNodes(nodesTrip);
            solution[indexRandomTrip]->setCurrentLength(lengthTrip);
            feasibleRandomSolution = true;

            // cout << "Gerou solução viavel" << endl;
            // cout << "index trip a ser modificada: " << indexRandomTrip << endl;
            // cout << "index no removido: " << indexRandomNodeRemover << endl;
            // cout << "id do no add: " << indexRandomVertexAdd << endl;
            // printTrips(data, solution);
            // cout << "Qualidade solução: " << getScoreTour(data, solution) << endl;
        }

        // cout <<"indexRandomTrip: "<<indexRandomTrip<<" indexRandomVertexAdd: "<<indexRandomVertexAdd<< " indexRandomNodeRemover: "<<indexRandomNodeRemover<<endl;
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
Trip **simulatedAnnealing(OPHS *data, Trip **initialSolution, int iterations, float T, float Tmin, mt19937 *gen)
{
    Trip **bestSolution = initialSolution;

    int iteracoesT = 0;
    while (T > Tmin)
    {
        int iter = 0;
        while (iter < iterations)
        {
            Trip **copySolution = makeCopySolution(data, initialSolution);
            Trip **neighborSolution = generateRandomNeighbor(data, copySolution, gen);

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
                float x = doubleRandom(0.0, 1.0, gen);
                // cout << "Temperatura T: " << T << "\t|";
                // cout << " deltaE: " << deltaE << "\t|";
                // cout << " probabilidade: " << p << "\t\t|";
                // cout << " Valor aleatorio: " << x << endl;
                if (x < p)
                {
                    initialSolution = neighborSolution;
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