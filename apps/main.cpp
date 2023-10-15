#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"
#include "util.h"
#include "simulated_annealing.h"

using namespace std;

int main()
{
    unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed); // gerador de números aleatórios com a semente
    cout << "SEED: " << seed << endl;

    string instance_folder = "../instances/OPHS_instances_February 2013/";
    string instance_name = "SET4/100-20-3-3";

    OPHS *data = read_input(instance_folder + instance_name + ".ophs");

    greedy_randomized_adaptive_reactive_procedure(data, &gen);

    printTrips(data, data->getTrips());
       
    // Trip** tour = load_solution("../out/"+instance_name+".ophsout", data);

    int iteracoes = 100;
    float temperaturaInicial = 100;
    float temperaturaFinal = 0.01;
    Trip **solucaoInicial = data->getTrips();
    float scoreSolInicial = getScoreTour(data, solucaoInicial);

    printTrips(data, solucaoInicial);

    cout << "Entrou simulated\n";
    cout << "SOLUCAO: " << scoreSolInicial << endl;
    Trip **novaSolucao = simulatedAnnealing(data, solucaoInicial, iteracoes, temperaturaInicial, temperaturaFinal, &gen);
   
    cout << "Saiu simulated\n";
    float scoreNovaSol = getScoreTour(data, novaSolucao);
    cout << "SOLUCAO SA: " << scoreNovaSol << endl;
    cout << "Melhora de : " << ((scoreNovaSol / scoreSolInicial) - 1) * 100 << " %" << endl;
    printTrips(data, novaSolucao);

    return 0;
}