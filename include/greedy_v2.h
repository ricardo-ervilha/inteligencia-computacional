#ifndef GREEDYV2_H
#define GREEDYV2_H

#include "OPHS.h"
#include <set>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include "util.h"

/* Preenchimento dos Hoteis Iniciais */

void print_matrix(OPHS *data, bool*** matrix){
    for(int k = 0; k < data->getNumTrips(); k++){
        cout << "Trip " << k << endl;
        for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
            for(int j = 0; j < data->getNumExtraHotels() + 2; j++){
                cout << boolalpha << matrix[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}


vector<vector<int>> recursive_fillable_possibilities(OPHS *data, bool*** matrix, vector<vector<int>> combinations, int nivel) {
    if (nivel == 0) {
        cout << "Nivel: " << nivel << endl;

        for (int j = 0; j < data->getNumExtraHotels() + 2; j++) {
            if (matrix[0][j][0]) {
                vector<int> sup;
                sup.push_back(0);
                sup.push_back(j);
                combinations.push_back(sup);
            }
        }

        return recursive_fillable_possibilities(data, matrix, combinations, nivel + 1);

    } else if (nivel == data->getNumTrips() - 1) {
        cout << "Nivel: " << nivel << endl;

        for (int i = 0; i < combinations.size(); i++) {
            if (matrix[combinations[i].back()][1][nivel]) {
                combinations[i].push_back(1);
            }
        }

        return combinations;
    } else {
        cout << "Nivel 1\n";
        vector<vector<int>> combinations2;
        for (int i = 0; i < combinations.size(); i++) {
            for (int j = 0; j < data->getNumExtraHotels() + 2; j++) {
                if (matrix[combinations[i].back()][j][nivel]) {
                    vector<int> new_vector = combinations[i];
                    new_vector.push_back(j);
                    combinations2.push_back(new_vector);
                }
            }
        }

        return recursive_fillable_possibilities(data, matrix, combinations2, nivel + 1);
    }
}

vector<vector<int>> mount_feasible_possibilities(OPHS *data, bool*** matrix) {
    vector<vector<int>> combinations;
    vector<vector<int>> *pointer = new vector<vector<int>>(recursive_fillable_possibilities(data, matrix, combinations, 0));

    // for (int i = 0; i < pointer->size(); i++) {
    //     vector<int> aux = (*pointer)[i];
    //     for (int j = 0; j < aux.size(); j++) {
    //         cout << aux[j] << " ";
    //     }
    //     cout << endl;
    // }

    // cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n";

    vector<vector<int>> aux;
    //Tratando para retirar combinações inválidas
    for(int i = 0; i < pointer->size(); i++){
        if((*pointer)[i].size() == data->getNumTrips() + 1){
            aux.push_back((*pointer)[i]);
        }
    }

    *pointer = aux;

    // Print das combinações
    for (int i = 0; i < pointer->size(); i++) {
        vector<int> aux = (*pointer)[i];
        for (int j = 0; j < aux.size(); j++) {
            cout << aux[j] << " ";
        }
        cout << endl;
    }

    return *pointer;
}

//Ordem de complexidade O(H*H*D)
void find_hotels_combination(OPHS* data, mt19937 *gen){
    
    //i -> hotel_inicio, j -> hotel_final, k -> qual trip está sendo considerada no momento
    bool ***matrix = new bool**[data->getNumExtraHotels() + 2];

    for (int i = 0; i < data->getNumExtraHotels() + 2; i++) {
        matrix[i] = new bool*[data->getNumExtraHotels() + 2];
        for (int j = 0; j < data->getNumExtraHotels() + 2; j++) {
            matrix[i][j] = new bool[data->getNumTrips()];
        }
    }

    for(int k = 0; k < data->getNumTrips(); k++){
        for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
            for(int j = 0; j < data->getNumExtraHotels() + 2; j++){
                float distance = data->getDistance(i, j);
                if(distance <= data->getTrip(k)->getTd())
                    matrix[i][j][k] = true;
                else
                    matrix[i][j][k] = false;
            }
        }
    }

    print_matrix(data, matrix);

    vector<vector<int>> hotels = mount_feasible_possibilities(data, matrix);
    
    int randomPossibilityIndex = intRandom(0, hotels.size() - 1, gen);
    cout << "Indice da combinacao: " << randomPossibilityIndex << endl;


    int cont = 0;
    while(cont < data->getNumTrips()){
        data->getTrip(cont)->setStartHotel(hotels[randomPossibilityIndex][cont]);
        data->getTrip(cont)->setEndHotel(hotels[randomPossibilityIndex][cont+1]);
        cont++;
    }

    for (int i = 0; i < data->getNumExtraHotels() + 2; i++) {
        for (int j = 0; j < data->getNumExtraHotels() + 2; j++) {
            delete[] matrix[i][j];
        }
        delete[] matrix[i];
    }

    delete[] matrix;
}

/* Abordagem Construtiva */

bool anotherCompareCandidatos(std::tuple<int, int, float, float> a, std::tuple<int,int, float, float> b)
{
    return std::get<2>(a) > std::get<2>(b);
}

vector<tuple<int, int, float, float>> generate_candidate_list_ap(OPHS *data, set<int> visiteds, int idTrip){
    //Ideia é verificar todas as posições pra inserção.
    
    vector<tuple<int, int, float, float>> candidatos;
    Trip* trip = data->getTrip(idTrip); //Trip de interesse no momento.
    //Para cada vértice (Nó).
    
    for(int i = data->getNumExtraHotels() + 2; i < data->getNumVertices() + data->getNumExtraHotels(); i++){
        //Se Nó não foi visitado.  
        if(visiteds.find(i) == visiteds.end()){
            //Se trip for vazia
            if(trip->getNodes().empty()){
                
                float incremento = data->getDistance(i, trip->getStartHotel());
                
                float distanceToEndHotel = data->getDistance(i, trip->getEndHotel());
                
                float newDistance = trip->getCurrentLength() + incremento + distanceToEndHotel;
                if(newDistance <= trip->getTd()){
                    float beneficio = (data->getVertex(i).score / (incremento) ) ;
                    tuple<int, int, float, float> candidato = std::make_tuple(i, 0, beneficio, incremento);
                    candidatos.push_back(candidato);
                }
            }else{
                //Quantidade de possibilidades para inserção é numero de nós + 1.
                for(int j = 0; j < trip->getNodes().size() + 1; j++){
                    
                    int anterior, posterior;
                    float incremento;
                    float distanceToEndHotel;
                    if(j == 0){
                        anterior = trip->getStartHotel();
                        posterior = trip->getNodes().front().id;
                        
                        incremento =  data->getDistance(anterior, i) + data->getDistance(i, posterior) - data->getDistance(anterior, posterior);

                        distanceToEndHotel = data->getDistance(trip->getNodes().back().id, trip->getEndHotel());

                    }else if(j == trip->getNodes().size()){
                        anterior = trip->getNodes().back().id;
                        posterior = trip->getEndHotel();
                        
                        incremento = data->getDistance(anterior, i);
                        distanceToEndHotel =  data->getDistance(i, posterior);

                    }else{
                        anterior = trip->getNode(j-1).id;
                        posterior = trip->getNode(j).id;
                        
                        incremento = data->getDistance(anterior, i) + data->getDistance(i, posterior) - data->getDistance(anterior,posterior);
                        distanceToEndHotel = data->getDistance(trip->getNodes().back().id, trip->getEndHotel());
                    }

                    float newDistance = trip->getCurrentLength() + incremento + distanceToEndHotel;

                    if(newDistance <= trip->getTd()){
                        float beneficio = (data->getVertex(i).score / (incremento) );
                        tuple<int, int, float, float> candidato = std::make_tuple(i, j, beneficio, incremento);
                        candidatos.push_back(candidato);
                    }
                }
            }
        }


    }

    std::sort(candidatos.begin(), candidatos.end(), anotherCompareCandidatos);
    // imprime_lista_candidatos(candidatos);
    
    return candidatos;
}

int randomRange(int num_candidatos, float alfa, mt19937 *gen)
{
    if(alfa == 0)   
        return 0;
    int mod = ceil(alfa * num_candidatos);
    return intRandom(0, num_candidatos, gen) % mod;
}


Trip** constructive_algorithm(OPHS *data, mt19937 *gen, float alfa) {

    set<int> visiteds; //Lista de visitados global
    
    for(int i = 0; i < data->getNumTrips();i++){
        
        //Passo as trips inicializadas com hoteis, passo visiteds, passo idTrip como i e o último hotel inserido nesse inicio vai ser o hotel de inicio.
        Trip *trip = data->getTrip(i);
        
        vector<tuple<int, int, float, float>> candidatos = generate_candidate_list_ap(data, visiteds, i);
        
        while(!candidatos.empty()){
            int index = randomRange(std::distance(candidatos.begin(), candidatos.end()), alfa, gen);
            
            Node good_node = data->getVertex(std::get<0>(candidatos[index]));
            trip->updateCurrentLength(std::get<3>(candidatos[index]));
            trip->add(good_node, std::get<1>(candidatos[index]));
            visiteds.insert(std::get<0>(candidatos[index]));
            
            candidatos = generate_candidate_list_ap(data, visiteds, i);
            
        }
        
        //Se a trip não está vazia (sem nós!!!!)      
        if(trip->getNodes().size() != 0)
            trip->updateCurrentLength(data->getDistance(trip->getNodes().back().id, trip->getEndHotel()));
        
    }

    return data->getTrips();
}


int sorteia_alfa(float *probAlfas, int tamAlfas){
    //Usando probabilidade acumulada.
    double random_number = rand() / (float)RAND_MAX;
    
    // cout << random_number << endl;
    
    float val_acumulado = 0;
    int i;
    for(i = 0; i < tamAlfas; i++){
        val_acumulado += probAlfas[i];
        if(random_number <= val_acumulado){
            return i;
        }
    }
    return i-1;
}

void recalculaQis(float *q, int* somaAlfas, int *numVezesAlfas, int tamAlfa, int scoreMelhor){
    int teta = 10;
    for(int i = 0; i < tamAlfa; i++){
        if(numVezesAlfas[i] != 0){
            float media = somaAlfas[i]/numVezesAlfas[i]; 
            q[i] = pow((scoreMelhor / media) , teta);
        }
    }
}

void atualizaProbabilidades(float *q, float *probabilidades, int tamAlfa){
    float somatorioQ = 0;
    for(int i = 0; i < tamAlfa; i++){
        somatorioQ += q[i];
    }

    for(int i = 0; i < tamAlfa; i++){
        probabilidades[i] = q[i] / somatorioQ; 
    }
}

void greedy_randomized_adaptive_reactive_procedure(OPHS *data, mt19937 *gen)
{   
    time_t val = time(NULL);
    srand(val);
    cout << "Seed do Randomizado Reativo: " << val << endl;

    find_hotels_combination(data, gen); //preenche o tour com os hoteis iniciais.

    Trip **copia = makeCopySolution(data, data->getTrips());
    Trip **melhor;
    int scoreMelhor;
    Trip** result;
    int scoreTotal;

    int tamAlfa = 7;
    float alfas[7] = {0.0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5}; 
    float probAlfas[7];
    int somaAlfas[7];
    int numVezesAlfas[7];
    float q[7];
    
    for(int i = 0; i < tamAlfa; i++){
        probAlfas[i] = 1/tamAlfa;
        somaAlfas[i] = 0;
        numVezesAlfas[i] = 0;
        q[i] = 0;
    }
    
    int maxIteracoes = 500;
    int y = maxIteracoes / 50; //Vai recalcular 5 vezes, a cada 10 iterações

    for(int i = 0; i < maxIteracoes; i++){
        int indice_alfa = sorteia_alfa(probAlfas, tamAlfa);
        
        result = constructive_algorithm(data, gen, alfas[indice_alfa]);
        
        scoreTotal = getScoreTour(data, result);
        
        somaAlfas[indice_alfa] += scoreTotal;
        numVezesAlfas[indice_alfa] += 1;
        
        //Atualiza melhor solução.
        if(i == 0){
            melhor = makeCopySolution(data, result);
            scoreMelhor = scoreTotal;
        }else{
            if(scoreTotal > scoreMelhor){
                melhor = makeCopySolution(data, result);
                scoreMelhor = scoreTotal;
            }
        }
        
        //Depois de rodar y vezes chega a hora de recalcular as probabilidades.
        if(i % y == 0){
            recalculaQis(q, somaAlfas, numVezesAlfas, tamAlfa, scoreMelhor);
            atualizaProbabilidades(q, probAlfas, tamAlfa);
            for(int i = 0; i < tamAlfa; i++){
                somaAlfas[i] = 0;
                numVezesAlfas[i] = 0;
            }
        }
        delete [] result;
        data->setTrips(copia);
        copia = makeCopySolution(data, data->getTrips());
        
    }
    
    data->setTrips(melhor);
    cout << "Seed do Randomizado Reativo: " << val << endl;
}


#endif