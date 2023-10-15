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
    string instance_name = "SET4/102-35-3-2";

    OPHS *data = read_input(instance_folder + instance_name + ".ophs");

    greedy_randomized_adaptive_reactive_procedure(data, &gen);

    // data->getTrip(0)->setStartHotel(0);
    // data->getTrip(0)->setEndHotel(2);
    // data->getTrip(1)->setStartHotel(2);
    // data->getTrip(1)->setEndHotel(1);

    // data->getTrip(0)->add(data->getVertex(3));
    // data->getTrip(0)->add(data->getVertex(5));
    // data->getTrip(0)->add(data->getVertex(8));
    // data->getTrip(0)->add(data->getVertex(12));
    // data->getTrip(0)->add(data->getVertex(17));
    // data->getTrip(0)->add(data->getVertex(24));
    // data->getTrip(0)->add(data->getVertex(31));
    // data->getTrip(0)->add(data->getVertex(23));
    // data->getTrip(0)->add(data->getVertex(30));
    // data->getTrip(0)->add(data->getVertex(38));
    // data->getTrip(0)->add(data->getVertex(45));
    // data->getTrip(0)->add(data->getVertex(39));
    // data->getTrip(0)->add(data->getVertex(32));
    // data->getTrip(0)->add(data->getVertex(40));
    // data->getTrip(0)->add(data->getVertex(33));
    // data->getTrip(0)->add(data->getVertex(34));

    // data->getTrip(1)->add(data->getVertex(42));
    // data->getTrip(1)->add(data->getVertex(35));
    // data->getTrip(1)->add(data->getVertex(28));
    // data->getTrip(1)->add(data->getVertex(36));
    // data->getTrip(1)->add(data->getVertex(29));
    // data->getTrip(1)->add(data->getVertex(37));
    // data->getTrip(1)->add(data->getVertex(44));
    // data->getTrip(1)->add(data->getVertex(50));
    // data->getTrip(1)->add(data->getVertex(43));
    // data->getTrip(1)->add(data->getVertex(49));
    // data->getTrip(1)->add(data->getVertex(55));
    // data->getTrip(1)->add(data->getVertex(59));
    // data->getTrip(1)->add(data->getVertex(62));
    // data->getTrip(1)->add(data->getVertex(64));

    printTrips(data, data->getTrips());
    cout << calcTripLength(data, data->getTrip(0)) << endl;
    cout << calcTripLength(data, data->getTrip(1)) << endl;    
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