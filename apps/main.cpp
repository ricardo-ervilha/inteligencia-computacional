#include <iostream>
#include "../include/file_io.h"
#include "../include/genetic.h"
#include "../include/greedy_v2.h"
#include "util.h"
#include "simulated_annealing.h"
#include <chrono>
#include <iomanip>

using namespace std;

/*
Parâmetros GENÉTICO
    ["SET1 1-2/64-45-1-2",  4, 0.5, 0.8, 1., 8],
    ["SET1 1-2/100-35-1-2", 1, 0, 0, 1., 1., numGenerations],
    
    ["SET1 2-3/66-125-2-3",  4, 0.4, 0.7, 0.95, 5, numGenerations],
    ["SET1 2-3/102-60-2-3",  6, 0.5, 0.7, 1., 10],
    
    ["SET1 3-4/64-70-3-4",  6, 0.5, 0.7, 0.9, 6],
    ["SET1 3-4/100-40-3-4",  1, 0.0, 0., 1., 1],
    
    ["SET2 5-3/66-60-5-3",  6, 0.6, 0.7, 0.95, 6],
    ["SET2 5-3/100-45-5-3",  3, 0.6, 0.7, 0.95, 3],
    
    ["SET2 6-4/66-50-6-4",  8, 0.8, 0.7, 0.95, 8],
    ["SET2 6-4/100-35-6-4",  1, 0.0, 0.0, 0.0, 1],
    
    ["SET3 10-4/66-125-10-4",  6, 0.8, 0.7, 0.8, 5],
    ["SET3 10-4/100-200-10-4",  5, 0.8, 0.7, 0.8, 5],
    
    ["SET3 12-5/64-75-12-5",  10, 0.8, 0.4, 0.8, 6],
    ["SET3 12-5/100-180-12-5",  9, 0.8, 0.5, 0.8, 5],
    
    ["SET4/100-20-3-3",  7, 0.8, 0.5, 0.8, 5],
    ["SET4/100-25-3-3",  7, 0.8, 0.5, 0.8, 8]
    ["SET4/102-35-3-3",  5, 0.8, 0.5, 0.9, 8]
    ["SET4/102-40-3-3",  5, 0.8, 0.5, 0.9, 8]
    ["SET4/102-45-3-3",  5, 0.8, 0.5, 0.9, 8]
*/

int main()
{
    // Parte de inicialização de sementes
    unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed); // gerador de números aleatórios com a semente
    cout << "SEED: " << seed << endl;

    time_t val = time(NULL);
    srand(val);

    // Parte de leitura da instância
    string instance_folder = "../instances/OPHS_instances_February 2013/";
    string instance_name = "SET4/102-45-3-3";
    string outputfile = "../out/" + instance_name + ".ophsout";

    OPHS *data = read_input(instance_folder + instance_name + ".ophs");

    //*****************************************************************************
    
    //Algoritmo construtivo
    // data->printDadosOPHS();

    // greedy_randomized_adaptive_reactive_procedure(data, &gen);

    // data->printDadosOPHS();

    // float scoreInicial = getScoreTour(data, data->getTrips());
    // cout << "+ Score Final do passeio: " << scoreInicial << endl;
    
    //*****************************************************************************

    //Parte do Algoritmo Genético
    find_hotels_combination(data, &gen); //Acha todas as possíveis combinações

    auto start = chrono::high_resolution_clock::now();

    cout << "TNFSH: " << data->getHoteisFeasible().size() << endl;
    Trip** best_solution = genetic_algorithm(data, 5, 0.8, 0.5, 0.9, 8, &gen);

    auto end = chrono::high_resolution_clock::now();

    double time_taken = 
      chrono::duration_cast<chrono::nanoseconds>(end - start).count();
 
    time_taken *= 1e-9;
 
    // cout << "Time taken by program is : " << fixed 
    //      << time_taken << setprecision(9);
    // cout << " sec" << endl;
    
    // cout << defaultfloat;

    data->setTrips(best_solution);
    data->printDadosOPHS();
    cout << "Score da Best Solution pós genético: " << getScoreTour(data, data->getTrips()) << endl;

    //*****************************************************************************

    // Parte do Simulated Annealing
    int iteracoes = 100;
    float temperaturaInicial = 100;
    float temperaturaFinal = 0.01;
    Trip **solucaoInicial = data->getTrips();

    Trip **novaSolucao = simulatedAnnealing(data, solucaoInicial, iteracoes, temperaturaInicial, temperaturaFinal, &gen);
    
    // writeTrips(data, solucaoInicial, outputfile);

    data->printDadosOPHS();

    float scoreFinal = getScoreTour(data, novaSolucao);
    cout << "+ Score Final do passeio: " << scoreFinal << endl;

    // cout << "Melhora de : " << ((scoreFinal / scoreInicial) - 1) * 100 << " %" << endl;

    // writeTrips(data, solucaoInicial, outputfile);

    // string filename = "saida.txt";
    // std::ofstream outputFile(filename, std::ofstream::app);
    // outputFile << instance_name<<"\t"<<scoreFinal<<"\t"<<duration.count()<<endl;
    // outputFile.close();

    return 0;
}
