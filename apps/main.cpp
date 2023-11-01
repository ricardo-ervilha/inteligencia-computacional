#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"
#include "util.h"
#include "simulated_annealing.h"

using namespace std;

int main()
{
    //Parte de inicialização de sementes
    unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed); // gerador de números aleatórios com a semente
    cout << "SEED: " << seed << endl;

    //Parte de leitura da instância
    string instance_folder = "../instances/OPHS_instances_February 2013/";
    string instance_name = "SET2 6-4/100-35-6-4";
    string outputfile = "../out/" + instance_name + ".ophsout";

    OPHS *data = read_input(instance_folder + instance_name + ".ophs");

    data->printDadosOPHS();

    //Parte do Algoritmo Construtivo
    greedy_randomized_adaptive_reactive_procedure(data, &gen);

    data->printDadosOPHS();
    
    float scoreInicial = getScoreTour(data, data->getTrips());
    cout << "+ Score Final do passeio: " << scoreInicial << endl;

    
    // Parte do Simulated Annealing
    int iteracoes = 100;
    float temperaturaInicial = 100;
    float temperaturaFinal = 0.01;
    Trip **solucaoInicial = data->getTrips();

    writeTrips(data, solucaoInicial, outputfile);
    Trip **novaSolucao = simulatedAnnealing(data, solucaoInicial, iteracoes, temperaturaInicial, temperaturaFinal, &gen);

    data->printDadosOPHS();

    float scoreFinal = getScoreTour(data, novaSolucao);
    cout << "+ Score Final do passeio: " << scoreFinal << endl;

    cout << "Melhora de : " << ((scoreFinal / scoreInicial) - 1) * 100 << " %" << endl;

    writeTrips(data, solucaoInicial, outputfile);
    return 0;
}
