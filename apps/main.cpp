#include <iostream>
#include "../include/file_io.h"
#include "../include/genetic.h"
#include "../include/greedy_v2.h"
#include "util.h"
#include "simulated_annealing.h"

using namespace std;

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
    string instance_name = "SET1 1-2/64-45-1-2";
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
    genetic_algorithm(data, 2, 0.25, 0.25, &gen);

    //*****************************************************************************

    // Parte do Simulated Annealing
    // int iteracoes = 100;
    // float temperaturaInicial = 100;
    // float temperaturaFinal = 0.01;
    // Trip **solucaoInicial = data->getTrips();

    // Trip **novaSolucao = simulatedAnnealing(data, solucaoInicial, iteracoes, temperaturaInicial, temperaturaFinal, &gen);
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "Tempo decorrido: " << duration.count() << " ms" << std::endl;

    // writeTrips(data, solucaoInicial, outputfile);

    // // data->printDadosOPHS();

    // float scoreFinal = getScoreTour(data, novaSolucao);
    // cout << "+ Score Final do passeio: " << scoreFinal << endl;

    // cout << "Melhora de : " << ((scoreFinal / scoreInicial) - 1) * 100 << " %" << endl;

    // writeTrips(data, solucaoInicial, outputfile);

    // string filename = "saida.txt";
    // std::ofstream outputFile(filename, std::ofstream::app);
    // outputFile << instance_name<<"\t"<<scoreFinal<<"\t"<<duration.count()<<endl;
    // outputFile.close();

    return 0;
}
