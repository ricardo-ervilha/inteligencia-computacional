#include <iostream>
#include "../include/file_io.h"
#include "../include/genetic.h"
#include "../include/greedy_v2.h"
#include "util.h"
#include "simulated_annealing.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 7)
    {
        cout << "Número de parâmetro inválido..." << endl;
        exit(0);
    }

    // Parte de inicialização de sementes
    unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed); // gerador de números aleatórios com a semente
    cout << "SEED: " << seed << endl;

    time_t val = time(NULL);
    srand(val);

    // Parte de leitura da instância
    string instance_folder = "../test_instances/";
    string instance_name = argv[1];
    string outputfile = "../out/" + instance_name + ".ophsoutSA";// solução final do SA
    string outputfile_ga = "../out/" + instance_name + ".ophsoutGA"; // solução do GA
    cout << "Nome da instancia: " << instance_name << endl;
    cout << "Saida: " << outputfile << endl;

    OPHS *data = read_input(instance_folder + instance_name + ".ophs");

    //*****************************************************************************

    // Algoritmo construtivo
    //  data->printDadosOPHS();

    // greedy_randomized_adaptive_reactive_procedure(data, &gen);
    // writeTrips(data, data->getTrips(), outputfile);

    // data->printDadosOPHS();

    // float scoreInicial = getScoreTour(data, data->getTrips());
    // cout << "+ Score Final do passeio: " << scoreInicial << endl;

    //*****************************************************************************

    // Parte do Algoritmo Genético
    // Parâmetros Genetico
    int tamPopInit = stoi(argv[2]);
    ;
    float percentualCrossOver = stof(argv[3]);
    float percentualMutacao = stof(argv[4]);
    float gama = stof(argv[5]);
    float numGenerations = stof(argv[6]);

    auto start_genetic = std::chrono::high_resolution_clock::now();

    Trip **best_solution = genetic_algorithm(data, tamPopInit, percentualCrossOver, percentualMutacao, gama, numGenerations, &gen);

    auto end_genetic = std::chrono::high_resolution_clock::now();
    auto duration_genetic = std::chrono::duration_cast<std::chrono::milliseconds>(end_genetic - start_genetic);

    data->setTrips(best_solution);
    writeTrips(data, best_solution, outputfile_ga);
    data->printDadosOPHS();
    float scoreGenetic = getScoreTour(data, data->getTrips());
    cout << "Score da Best Solution pós genético: " << scoreGenetic << endl;

    //*****************************************************************************

    // Carregando uma solução gravada na pasta out, caso não exista, cria uma
    // load_solution(outputfile, data));

    // Parte do Simulated Annealing
    int iteracoes = 100;
    float temperaturaInicial = 100;
    float temperaturaFinal = 0.01;
    Trip **solucaoInicial = data->getTrips();

    auto start_sa = std::chrono::high_resolution_clock::now();

    Trip **novaSolucao = simulatedAnnealing(data, solucaoInicial, iteracoes, temperaturaInicial, temperaturaFinal, &gen);

    auto end_sa = std::chrono::high_resolution_clock::now();
    auto duration_sa = std::chrono::duration_cast<std::chrono::milliseconds>(end_sa - start_sa);
    auto duration_total = duration_genetic.count() + duration_sa.count();

    data->setTrips(novaSolucao);
    writeTrips(data, novaSolucao, outputfile);
    data->printDadosOPHS();

    float scoreFinal = getScoreTour(data, novaSolucao);
    cout << "+ Score Final do passeio: " << scoreFinal << endl;

    string filename = "../out/metrics.csv";
    std::ofstream outputFile(filename, std::ofstream::app);

    outputFile << instance_name << "," <<  // nome_instancia
        seed << "," <<                     // Semente 1
        val << "," <<                      // Semente 2
        scoreGenetic << "," <<             // Score Genetic
        duration_genetic.count() << "," << // Tempo Genetic
        scoreFinal << "," <<               // Score SA
        duration_sa.count() << "," <<      // Tempo SA
        duration_total <<                  // Tempo total
        endl;
    outputFile.close();

    return 0;
}
