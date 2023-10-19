#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H
#include "util.h"
#include "local_search.h"
#include <cmath>
#include <random>
#include <set>

using namespace std;

void printCandidatos(set<tuple<int, int, int>> candidatosGerados)
{
    for (const auto &tupla : candidatosGerados)
    {
        int valor1, valor2, valor3;
        std::tie(valor1, valor2, valor3) = tupla;
        std::cout << "Trip " << valor1 << " No " << valor2 << " random " << valor3 << std::endl;
    }
}

void printCandidatos(vector<tuple<int, int, int, float, float>> candidatosGerados)
{
    for (const auto &tupla : candidatosGerados)
    {
        int indexTrip, indexNoRemover, idVertexAdd;
        float novoScore, novaDistancia;
        std::tie(indexTrip, indexNoRemover, idVertexAdd, novoScore, novaDistancia) = tupla;
        std::cout << "Trip " << indexTrip << " IndexNoRemover " << indexNoRemover << " idVertexAdd " << idVertexAdd << " NovoScore: " << novoScore << " NovaDistancia: " << novaDistancia << std::endl;
    }

    if (candidatosGerados.size() == 0)
        cout << "Ninguem na lista..." << endl;
}

// <indexTrip, indexNo, idxVertexAdd, incrementoDistancia, ratio = score/incrementoDistancia>
void printCandidatosInsert(vector<tuple<int, int, int, float, float>> candidatosGerados)
{
    for (const auto &tupla : candidatosGerados)
    {
        int indexTrip, indexNo, idVertexAdd;
        float incrementoDistancia, ratio;
        tie(indexTrip, indexNo, idVertexAdd, incrementoDistancia, ratio) = tupla;

        cout << "Trip " << indexTrip;
        cout << " indexNo " << indexNo;
        cout << " idVertexAdd " << idVertexAdd;
        cout << " incrementoDistancia: " << incrementoDistancia;
        cout << " ratio " << ratio << endl;
    }
}

float updateTemperature(float T)
{
    return 0.99 * T;
}

Trip **applyRandomMovement(OPHS *data, Trip **neighborSolution, mt19937 *gen)
{
    /*
        0. generateRandomNeighbor
        1. twoOpt
        2. generateRandomNeighbor2
        3. insert
    */
    set<int> vazio;
    int randomMovement = intRandom(0, 3, gen);
    // cout <<"randomMovement: "<<randomMovement<<endl;
    if (randomMovement == 0)
    {
        // cout << "Aplicando generateRandomNeighbor" << endl;
        return generateRandomNeighbor(data, neighborSolution, gen);
    }

    if (randomMovement == 1)
    {
        // cout << "Aplicando twoOpt" << endl;
        return twoOpt(data, neighborSolution, gen);
    }

    if (randomMovement == 2)
    {
        // cout << "Aplicando generateRandomNeighbor2" << endl;
        return generateRandomNeighbor2(data, neighborSolution, gen);
    }

    if (randomMovement == 3)
    {
        // cout << "Aplicando insert: " << endl;
        Trip **solCopia = makeCopySolution(data, neighborSolution);
        Trip **sol = insert(data, neighborSolution, gen, vazio);

        if (sol == nullptr)
            return solCopia;
        else
            return sol;
    }
    return nullptr;
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
    set<int> vazio;

    int iteracoesT = 0;
    while (T > Tmin)
    {
        int iter = 0;
        while (iter < iterations)
        {
            Trip **copySolution = makeCopySolution(data, initialSolution);

            // cout << "ANTES EXTRACT: " << endl;
            // printIdsTrips(data, copySolution);

            Trip **neighborSolution = extract2Insert(data, copySolution, gen);
            neighborSolution = applyRandomMovement(data, neighborSolution, gen);
            // neighborSolution = applyRandomMovement(data, neighborSolution, gen);
            // neighborSolution = insert(data, neighborSolution, gen, vazio);
            // neighborSolution = generateRandomNeighbor2(data, neighborSolution, gen);
            // neighborSolution = exchange(data, neighborSolution, gen);
            // neighborSolution = extract2Insert(data, neighborSolution, gen);
            // cout << "DEPOIS EXTRACT: " << endl;
            // printIdsTrips(data, neighborSolution);

            float neighborSolutionScore = getScoreTour(data, neighborSolution);
            float initialSolutionScore = getScoreTour(data, initialSolution);

            // if (neighborSolutionScore == initialSolutionScore)
            // {
            //     // cout << "Mesma qualidade: "<<iter<< endl;
            //     if (equalSolution(data, initialSolution, neighborSolution))
            //     {
            //         // cout << "Mesma solução: " << iter << endl;
            //         neighborSolution = applyRandomMovement(data, neighborSolution, gen);
            //     }
            // }

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
                    cout << "Achou sol melhor..."
                         << "iter: " << iter << " Qualidade: " << neighborSolutionScore << endl;
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