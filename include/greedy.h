#ifndef GREEDY_H
#define GREEDY_H

#include <tuple>
#include "../src/gdsa_vector.cpp"

// bool feasible_solution(Trip **tour){
//     return;
// }

bool compareCandidatos(std::tuple<Node*, float, float> a, std::tuple<Node*, float, float> b)
{
    return std::get<1>(a) < std::get<1>(b);
}

std::forward_list<std::tuple<Node*, float, float>> generateCandidateList(OPHS *data, int cont, Node* last)
{
    std::forward_list<std::tuple<Node*, float, float>> candidatos;

    for(int i = 0; i < data->getNumVertices(); i++){
        if(data->getVertex()[i].vis == false){
            if(data->getTrips()[cont]->getCurrentLength() + data->getDistance(data->getVertex()[i].id, last->id) < data->getTrips()[cont]->getTd()){
                float distance = data->getDistance(data->getVertex()[i].id, last->id);
                float beneficio = data->getVertex()[i].score / distance;
                std::tuple<Node *, float, float> candidato = std::make_tuple(&data->getVertex()[i], beneficio, distance);
                candidatos.push_front(candidato);
            }
        }
    }

    candidatos.sort(compareCandidatos);
    return candidatos;
}

Trip** greedy_algorithm(OPHS *data){

    int cont = 0;

    std::forward_list<std::tuple<Node*, float, float>> cl;
     
    while (cont < data->getNumTrips())
    {
        while(true){
            cout << "Erro em GenerateCandidateList\n";
            cl = generateCandidateList(data, cont, data->getTrips()[cont]->front()); //gera candidatos viaveis
            cout << "Fim Erro em GenerateCandidateList\n"; //Erro linha 46 (!!!)

            int length = std::distance(cl.begin(), cl.end());
            cout <<"Dist: "<<length<<endl;
            if(length == 0) //Ninguém mais pode ser adicionado na trip
                break;
            
            std::tuple<Node*, float, float> vai_entrar = cl.front();
            cl.pop_front();
            std::get<0>(vai_entrar)->vis = true;
            data->getTrips()[cont]->updateCurrentLength(std::get<2>(vai_entrar));
            data->getTrips()[cont]->add(std::get<0>(vai_entrar));

            if(length == 1) //Ninguém mais pode ser adicionado na trip
                break;
        }

        cont++;
    }
    
    return data->getTrips();
} 

#endif