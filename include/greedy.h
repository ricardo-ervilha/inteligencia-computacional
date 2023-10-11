#ifndef GREEDY_H
#define GREEDY_H

#include "OPHS.h"
#include <set>
#include <tuple>
#include <algorithm>

typedef struct{
    float beneficio;
    int nodeToInsert;
    int bestPlaceToInsert;
} Candidate;

bool compareCandidatos(std::tuple<int, float, float> a, std::tuple<int,float, float> b)
{
    return std::get<1>(a) > std::get<1>(b);
}

void imprime_lista_candidatos(vector<tuple<int,float, float>> candidatos){
    cout << "--------------Lista de Candidatos--------------\n";
    for(int i = 0; i < candidatos.size(); i++){
        cout << std::get<0>(candidatos[i]) << " " << std::get<1>(candidatos[i]) << " " << std::get<2>(candidatos[i]) << endl; 
    }
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
            float beneficio = data->getVertex(i).score / (data->getDistance(idAnterior, i) + data->getDistance(i, trip->getEndHotel()));
            tuple<int, float, float> tupla = std::make_tuple(i, beneficio, incremento);
            candidatos.push_back(tupla);
        }
    }

    std::sort(candidatos.begin(), candidatos.end(), compareCandidatos);

    return candidatos;
}

//ap -> after pre-processing
// vector<Candidate> generate_candidate_list_ap(OPHS *data, int idTrip){
//     //Ideia é verificar todas as posições pra inserção.
//     //Para cada vértice (Nó).
//     for(int i = data->getNumExtraHotels() + 2; i <= data->getNumVertices() + data->getNumExtraHotels(); i++){
//         //Para cada lugar para inserir (índice).
//         for(int j = 1; j < data->getTrip(idTrip)->getNodes().size(); j++){
//             //Retirar a aresta que existia e adicionar duas novas.
//             float new_distance_to_verify = data->getTrip(idTrip)->getCurrentLength() - data->getDistance(j-1, j) + data->getDistance(i, j-1) + data->getDistance(i, j); 
//         }
//     }
// }

void pre_processing(OPHS *data){
    set<int> visiteds;
    Trip *trip = new Trip(data->getTrip(0)->getTd());
    trip->setStartHotel(2);
    trip->setEndHotel(2); 
    
    vector<tuple<int,float, float>> candidatos = generate_candidate_list_ip(data, visiteds, trip);
    
    while(!candidatos.empty()){
        Node good_node = data->getVertex(std::get<0>(candidatos.front()));
        
        int idAnterior;
        if(trip->getNodes().empty())
            idAnterior = trip->getStartHotel();
        else
            idAnterior = trip->getNodes().back().id;
        trip->updateCurrentLength(data->getDistance(idAnterior, std::get<0>(candidatos.front())));
        
        trip->add(good_node);

        visiteds.insert(std::get<0>(candidatos.front()));
        candidatos = generate_candidate_list_ip(data, visiteds, trip);
        
    }
    trip->updateCurrentLength(data->getDistance(trip->getNodes().back().id, trip->getEndHotel()));
    trip->dadosTrip();
    trip->dadosNodes();
    cout << "Total score: " << trip->getScoreTrip() << endl;
}

// void constructive_algorithm(OPHS *data) {

//     while(){

//     }


// }


#endif
