#ifndef GREEDY_H
#define GREEDY_H

#include "OPHS.h"
#include <set>
#include <tuple>
#include <algorithm>
#include <chrono>

bool compareCandidatos(std::tuple<int, float, float> a, std::tuple<int,float, float> b)
{
    return std::get<1>(a) > std::get<1>(b);
}

bool anotherCompareCandidatos(std::tuple<int, int, float> a, std::tuple<int,int, float> b)
{
    return std::get<2>(a) > std::get<2>(b);
}

void imprime_lista_candidatos(vector<tuple<int,int, float>> candidatos){
    
    cout << "--------------Lista de Candidatos--------------\n";
    for(int i = 0; i < candidatos.size(); i++){
        cout << std::get<0>(candidatos[i]) << " " << std::get<1>(candidatos[i]) << " " << std::get<2>(candidatos[i]) << endl; 
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

//ap -> after pre-processing
vector<tuple<int, int, float>> generate_candidate_list_ap(OPHS *data,set<int> visiteds, int idTrip, int lastInsert){
    vector<tuple<int, int, float>> candidatos;
    //Ideia é verificar todas as posições pra inserção.
    //Para cada vértice (Nó).
    for(int i = data->getNumExtraHotels() + 2; i <= data->getNumVertices() + data->getNumExtraHotels(); i++){
        //Para cada lugar para inserir (índice). Depois do primeiro nó que é o hotel até ser inserido depois do último nó.
        if(visiteds.find(i) == visiteds.end()){
            for(int j = 1; j < data->getTrip(idTrip)->getNodes().size(); j++){
                //Retirar a aresta que existia e adicionar duas novas.
                float new_distance_to_verify = data->getTrip(idTrip)->getCurrentLength() - data->getDistance(j-1, j) + data->getDistance(i, j-1) + data->getDistance(i, j);
                float distanceToEndHotel;
                if(j != data->getTrip(idTrip)->getNodes().size() - 1)
                    distanceToEndHotel += data->getDistance(data->getTrip(idTrip)->back().id, data->getTrip(idTrip)->getEndHotel());
                else
                    distanceToEndHotel += data->getDistance(j, data->getTrip(idTrip)->getEndHotel());
                new_distance_to_verify += distanceToEndHotel;
                if(new_distance_to_verify < data->getTrip(idTrip)->getTd()){
                    float beneficio = data->getVertex(i).score / (-data->getDistance(j-1, j) + data->getDistance(i, j-1) + data->getDistance(i, j) + distanceToEndHotel);
                    tuple<int, int, float> tupla = std::make_tuple(i,j, beneficio);
                    candidatos.push_back(tupla);
                } 
            }
        }
    }

    std::sort(candidatos.begin(), candidatos.end(), anotherCompareCandidatos);
    imprime_lista_candidatos(candidatos);
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

    vector<int> hoteis = construct_initial_tour_forward(data, matrix_pre_processing);
    data->getTrip(0)->setStartHotel(0);
    data->getTrip(0)->setEndHotel(hoteis[1]);
    int cont = 1;
    
    while(cont < data->getNumTrips()){
        data->getTrip(cont)->setStartHotel(hoteis[cont+1]);
        data->getTrip(cont)->setEndHotel(hoteis[cont+2]);
        cont++;
    }
    // construct_initial_tour_backward(data, matrix_pre_processing);
    
}
//Impressão dos dados
// trip->dadosTrip();
// trip->dadosNodes();
// cout << "Total score: " << trip->getScoreTrip() << endl;

void constructive_algorithm(OPHS *data) {
    pre_processing(data); //preenche o tour com os hoteis.

    set<int> visiteds;
    for(int i = 0; i < data->getNumTrips();i++){
        vector<tuple<int, int, float>> candidatos = generate_candidate_list_ap(data, visiteds, i, data->getTrip(i)->getStartHotel());
        while(!candidatos.empty()){
            Node good_node = data->getVertex(std::get<0>(candidatos.front()));

            int idAnterior;
            if (data->getTrip(i)->getNodes().empty())
                idAnterior = data->getTrip(i)->getStartHotel();
            else
                idAnterior = data->getTrip(i)->getNodes().back().id;
            data->getTrip(i)->updateCurrentLength(-data->getDistance(std::get<1>(candidatos.front())-1, std::get<1>(candidatos.front())) + data->getDistance(std::get<0>(candidatos.front()), std::get<1>(candidatos.front())-1) + data->getDistance(std::get<0>(candidatos.front()), std::get<1>(candidatos.front())));
            data->getTrip(i)->add(good_node);
            visiteds.insert(std::get<0>(candidatos.front()));
        }
                
        if(data->getTrip(i)->getNodes().size() != 0)
            data->getTrip(i)->updateCurrentLength(data->getDistance(data->getTrip(i)->getNodes().back().id, data->getTrip(i)->getEndHotel()));
    }
    
    for(int i = 0; i < data->getNumTrips(); i++){
        data->getTrip(i)->dadosTrip();
        data->getTrip(i)->dadosNodes();
    }
}


#endif
