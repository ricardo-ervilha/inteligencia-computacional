#ifndef GREEDY_H
#define GREEDY_H

#include "OPHS.h"
#include <set>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include "util.h"

bool compareCandidatos(std::tuple<int, float, float> a, std::tuple<int,float, float> b)
{
    return std::get<1>(a) > std::get<1>(b);
}

bool anotherCompareCandidatos(std::tuple<int, int, float, float> a, std::tuple<int,int, float, float> b)
{
    return std::get<2>(a) > std::get<2>(b);
}

void imprime_lista_candidatos(vector<tuple<int,int, float, float>> candidatos){
    
    cout << "--------------Lista de Candidatos--------------\n";
    for(int i = 0; i < candidatos.size(); i++){
        cout << std::get<0>(candidatos[i]) << " " << std::get<1>(candidatos[i]) << " " << std::get<2>(candidatos[i]) << " " << std::get<3>(candidatos[i]) << endl; 
    }
    
}

vector<int> construct_initial_tour_backward(OPHS *data, int ***matrix){
    vector<int> hoteis;
    hoteis.push_back(1); //Inicializando com zero.
    int max = matrix[1][0][data->getNumTrips()-1]; //Fixando hotel inicial como zero na trip 0. 
    int save_i = 0;
    for(int i = 1; i < data->getNumExtraHotels() + 2; i++){
        if(matrix[i][1][data->getNumTrips()-1] > max)
        {
            max = matrix[i][1][data->getNumTrips()-1];
            save_i = i;
        }
    }
    hoteis.push_back(save_i);
    hoteis.push_back(save_i);
    int cont = data->getNumTrips()-2;
    int save_j = save_i;
    //irá parar faltando preencher a trip final  
    while(cont >= 2){
        max = matrix[0][save_j][cont];
        
        for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
            if(matrix[i][save_j][cont] > max)
            {
                max = matrix[i][save_j][cont];
                save_i = i;
            }
        }

        hoteis.push_back(save_i);
        hoteis.push_back(save_i);
        cont--;
    }
    hoteis.push_back(0);
    reverse(hoteis.begin(), hoteis.end());
    cout << "Tour Backward: ";
    for(int i = 0; i < hoteis.size(); i++){
        if(i % 2 == 0)
            cout << hoteis[i] << " ";
        else
        {
            if(i != hoteis.size()-1)
                cout << hoteis[i] << ", ";
            else
                cout << hoteis[i] << endl;
        }            
    }

    return hoteis;
}

vector<int> construct_initial_tour_forward(OPHS *data, int ***matrix){
    vector<int> hoteis;
    hoteis.push_back(0); //Inicializando com zero.
    int max = matrix[0][0][0]; //Fixando hotel inicial como zero na trip 0. 
    int save_j = 0;
    for(int j = 1; j < data->getNumExtraHotels() + 2; j++){
        if(matrix[0][j][0] > max)
        {
            max = matrix[0][j][0];
            save_j = j;
        }
    }
    hoteis.push_back(save_j);
    hoteis.push_back(save_j);
    int cont = 1;
    int save_i = save_j;
    //irá parar faltando preencher a trip final  
    while(cont < data->getNumTrips() - 2){
        max = matrix[save_i][0][cont];
        
        for(int j = 0; j < data->getNumExtraHotels() + 2; j++){
            if(matrix[save_i][j][cont] > max)
            {
                max = matrix[save_i][j][cont];
                save_j = j;
            }
        }

        hoteis.push_back(save_j);
        hoteis.push_back(save_j);
        cont++;
    }
    hoteis.push_back(1);
    cout << "Tour Forward: ";
    for(int i = 0; i < hoteis.size(); i++){
        if(i % 2 == 0)
            cout << hoteis[i] << " ";
        else
        {
            if(i != hoteis.size()-1)
                cout << hoteis[i] << ", ";
            else
                cout << hoteis[i] << endl;
        }            
    }

    return hoteis;

}

//ip -> in pre-processing
vector<tuple<int,float, float>> generate_candidate_list_ip(OPHS* data, set<int> visiteds, Trip *trip){
    vector<tuple<int,float, float>> candidatos;
    int idAnterior;

    if(trip->getNodes().empty())
        idAnterior = trip->getStartHotel();
    else
        idAnterior = trip->getNodes().back().id;


    for(int i = data->getNumExtraHotels() + 2; i < data->getNumExtraHotels() + data->getNumVertices(); i++){

        float incremento = data->getDistance(idAnterior, i);
        float distanceToEndHotel = data->getDistance(i, trip->getEndHotel());
        float updated_distance = trip->getCurrentLength() + incremento + distanceToEndHotel;
        
        //Nó não foi visitado e a distância dele até o hotel final é menor que Td.
        if(visiteds.find(i) == visiteds.end() && updated_distance <= trip->getTd()){
            float beneficio = data->getVertex(i).score / (incremento + distanceToEndHotel);
            tuple<int, float, float> tupla = std::make_tuple(i, beneficio, incremento);
            candidatos.push_back(tupla);
        }
    }

    std::sort(candidatos.begin(), candidatos.end(), compareCandidatos);
    
    return candidatos;
}


//Ordem de complexidade O(H*H*D)
void pre_processing(OPHS *data){
    //i -> hotel_inicio, j -> hotel_final, k -> qual trip está sendo avaliado
    int ***matrix_pre_processing = new int**[data->getNumExtraHotels() + 2];
    
    for (int i = 0; i < data->getNumExtraHotels() + 2; i++) {
        matrix_pre_processing[i] = new int*[data->getNumExtraHotels() + 2];
        for (int j = 0; j < data->getNumExtraHotels() + 2; j++) {
            matrix_pre_processing[i][j] = new int[data->getNumTrips()];
        }
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for(int k = 0; k < data->getNumTrips(); k++){
        for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
            for(int j = 0; j < data->getNumExtraHotels() + 2; j++){
                
                set<int> visiteds;
                Trip *trip = new Trip(data->getTrip(k)->getTd());
                trip->setStartHotel(i);
                trip->setEndHotel(j);
                
                vector<tuple<int, float, float>> candidatos = generate_candidate_list_ip(data, visiteds, trip);
                
                while (!candidatos.empty())
                {
                    Node good_node = data->getVertex(std::get<0>(candidatos.front()));

                    int idAnterior;
                    if (trip->getNodes().empty())
                        idAnterior = trip->getStartHotel();
                    else
                        idAnterior = trip->getNodes().back().id;
                    trip->updateCurrentLength(data->getDistance(idAnterior, std::get<0>(candidatos.front())));
                    trip->add(good_node);
                    visiteds.insert(std::get<0>(candidatos.front()));
                    
                    candidatos = generate_candidate_list_ip(data, visiteds, trip);
                }
                
                if(trip->getNodes().size() != 0)
                    trip->updateCurrentLength(data->getDistance(trip->getNodes().back().id, trip->getEndHotel()));
                matrix_pre_processing[i][j][k] = trip->getScoreTrip(); //Salvo a pontuação total nessa matriz
                
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Tempo decorrido: " << duration.count() << " ms" << std::endl;
    for(int k = 0; k < data->getNumTrips(); k++){
        cout << "Trip: " << k << endl;
        for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
            for(int j = 0; j < data->getNumExtraHotels() + 2; j++){
                cout << matrix_pre_processing[i][j][k] << " ";
            }
            cout << endl;
        }
        cout << endl;
        cout << endl;
    }    

    data->getTrip(0)->setStartHotel(0);
    data->getTrip(0)->setEndHotel(4);
    data->getTrip(1)->setStartHotel(4);
    data->getTrip(1)->setEndHotel(1);
    // vector<int> hoteis = construct_initial_tour_backward(data, matrix_pre_processing);
    // data->getTrip(0)->setStartHotel(0);
    // data->getTrip(0)->setEndHotel(hoteis[1]);
    // int cont = 1;
    
    // while(cont < data->getNumTrips()){
    //     data->getTrip(cont)->setStartHotel(hoteis[cont+1]);
    //     data->getTrip(cont)->setEndHotel(hoteis[cont+2]);
    //     cont++;
    // }

    for (int i = 0; i < data->getNumExtraHotels() + 2; i++) {
        for (int j = 0; j < data->getNumExtraHotels() + 2; j++) {
            delete[] matrix_pre_processing[i][j];
        }
        delete[] matrix_pre_processing[i];
    }
    delete[] matrix_pre_processing;
    // construct_initial_tour_backward(data, matrix_pre_processing);
    
}
//Impressão dos dados
// trip->dadosTrip();
// trip->dadosNodes();
// cout << "Total score: " << trip->getScoreTrip() << endl;

//ap -> after pre-processing
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
                    float beneficio = data->getVertex(i).score / (incremento + distanceToEndHotel);
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
                        float beneficio = data->getVertex(i).score / (incremento + distanceToEndHotel) ;
                        tuple<int, int, float, float> candidato = std::make_tuple(i, j, beneficio, incremento);
                        candidatos.push_back(candidato);
                    }
                }
            }
        }


    }

    std::sort(candidatos.begin(), candidatos.end(), anotherCompareCandidatos);
    imprime_lista_candidatos(candidatos);
    
    return candidatos;
}


int randomRange(int num_candidatos, float alfa, mt19937 *gen)
{
    if(alfa == 0)   
        return 0;
    int mod = ceil(alfa * num_candidatos);
    return intRandom(0, num_candidatos, gen) % mod;
}

void avalia_hoteis(OPHS *data){
    for(int i = 0; i < data->getNumTrips()-1; i++){
        Trip *avaliada = data->getTrip(i);
        float min = avaliada->getCurrentLength() + data->getDistance(avaliada->getNodes().back().id, 0);
        int indice = 0;
        for(int j = 1; j < data->getNumExtraHotels() + 2; j++){
            float new_min = avaliada->getCurrentLength() + data->getDistance(avaliada->getNodes().back().id, j);
            if(new_min < min){
                min = new_min;
                indice = j;
            }
        }
        avaliada->setEndHotel(indice);
        data->getTrip(i+1)->setStartHotel(indice);
    }
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
            cout << "Valor do length: " << trip->getCurrentLength() << endl;
            trip->updateCurrentLength(std::get<3>(candidatos[index]));
            trip->add(good_node, std::get<1>(candidatos[index]));
            visiteds.insert(std::get<0>(candidatos[index]));
            avalia_hoteis(data);
            candidatos = generate_candidate_list_ap(data, visiteds, i);
        }
        //Se a trip não está vazia (sem nós!!!!)      
        if(trip->getNodes().size() != 0)
            trip->updateCurrentLength(data->getDistance(trip->getNodes().back().id, trip->getEndHotel()));
    }
    int totalScore = 0;
    for(int i = 0; i < data->getNumTrips(); i++){
        cout << data->getTrip(i)->getStartHotel() << " " << data->getTrip(i)->getEndHotel() << endl;
        data->getTrip(i)->dadosTrip();
        data->getTrip(i)->dadosNodes();
        cout << "Score: " << data->getTrip(i)->getScoreTrip()<<endl;
        totalScore +=  data->getTrip(i)->getScoreTrip();
    }
    cout << totalScore << endl;

    return data->getTrips();
}

int sorteia_alfa(float *probAlfas, int tamAlfas){
    //Usando probabilidade acumulada.
    double random_number = rand() / (float)RAND_MAX;
    
    cout << random_number << endl;
    
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
    cout << "Seed do GRARP: " << val << endl;

    pre_processing(data); //preenche o tour com os hoteis iniciais.

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
    
    int maxIteracoes = 200;
    int y = maxIteracoes / 10; //Vai recalcular 5 vezes, a cada 10 iterações

    for(int i = 0; i < maxIteracoes; i++){
        int indice_alfa = sorteia_alfa(probAlfas, tamAlfa);
        cout << "Craque\n";
        cout << indice_alfa << endl;
        cout << alfas[indice_alfa] << endl;
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
    printTrips(data, melhor);
    cout << "AAAH: " << getScoreTour(data, melhor) << endl;
    data->setTrips(melhor);
    cout << "Score do calabreso: " << scoreMelhor << endl;
}

#endif
