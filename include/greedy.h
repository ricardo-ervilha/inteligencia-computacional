#ifndef GREEDY_H
#define GREEDY_H

#include <tuple>
#include "../src/gdsa_vector.cpp"

// bool feasible_solution(Trip **tour){
//     return;
// }

bool compareCandidatos(std::tuple<Node*, float, float> a, std::tuple<Node*, float, float> b)
{
    return std::get<1>(a) > std::get<1>(b);
}

std::forward_list<std::tuple<Node*, float, float>> generateCandidateList(OPHS *data, int cont, Node* last)
{
    std::forward_list<std::tuple<Node*, float, float>> candidatos;

    //computo primeiro os dos hoteis
    for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
        if(data->getVertex()[i].vis == false){
            float distance = data->getDistance(data->getVertex()[i].id, last->id);
            float beneficio = 0;
            std::tuple<Node *, float, float> candidato = std::make_tuple(&data->getVertex()[i], beneficio, distance);
            candidatos.push_front(candidato);
        }
    }

    //Computo depois os dos nós
    for(int i = data->getNumExtraHotels() + 2; i < data->getNumExtraHotels() + data->getNumVertices(); i++){
        if(data->getVertex()[i].vis == false && data->getVertex()[i].id != 1){
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

void print_lista_de_candidatos(std::forward_list<std::tuple<Node*, float, float>> candidatos){
    for (std::forward_list<std::tuple<Node*, float, float>>::iterator it = candidatos.begin(); it != candidatos.end(); ++it){
        cout << "Id do no: " << std::get<0>(*it)->id << " Beneficio: " << std::get<1>(*it) << " Distancia: " << std::get<2>(*it) << endl;
    }
    cout << "****************************************" << endl;
}

Trip** greedy_algorithm(OPHS *data){

    int cont = 0;

    std::forward_list<std::tuple<Node*, float, float>> cl;
     
    while (cont < data->getNumTrips())
    {
        if(cont != 0){
            //adiciono na próxima trip o hotel final da última
            data->getTrips()[cont]->add(data->getTrips()[cont-1]->back());
        }

        bool flag = false;
        while(true){

            float margin = data->getTrips()[cont]->getTd() - data->getTrips()[cont]->getCurrentLength();

            int j;
            for(int i = 0; i < data->getNumExtraHotels() + 2; i++){
                if(data->getVertex()[i].vis == false){
                    if(data->getTrips()[cont]->getCurrentLength() + data->getDistance(data->getVertex()[i].id, data->getTrips()[cont]->front()->id) > data->getTrips()[cont]->getTd()){
                        j = i;
                        flag = true;
                        break;
                    }
                }
            }

            if(flag){
                data->getTrips()[cont]->add(&data->getVertex()[j-1]);
                break;
            }

            cout << "Trip de índice: " << cont << endl;
            data->getTrips()[cont]->print();
           
            cl = generateCandidateList(data, cont, data->getTrips()[cont]->front()); //gera candidatos viaveis
            
            print_lista_de_candidatos(cl);

            int length = std::distance(cl.begin(), cl.end());

            if(length == 0) //Ninguém mais pode ser adicionado na trip
                break;
            
            std::tuple<Node*, float, float> vai_entrar = cl.front();
            cl.pop_front();
            std::get<0>(vai_entrar)->vis = true;
            data->getTrips()[cont]->updateCurrentLength(std::get<2>(vai_entrar));
            data->getTrips()[cont]->add(std::get<0>(vai_entrar));

            //Beneficio do no que vai entrar ser zero, significa que só tem hoteis para serem adicionados na solucao
            if(std::get<1>(vai_entrar) == 0 && cont == data->getNumTrips() - 1){
                data->getTrips()[data->getNumTrips() - 1]->add(&data->getVertex()[1]);
                if(std::get<1>(vai_entrar) != 0) //hotel não precisa marcar como visitado
                    data->getVertex()[1].vis = true; //Adiciona o nó final na trip final.
            }
            else if(std::get<1>(vai_entrar) == 0){
                break; //Adicionou um hotel e acabou a trip
            }
        }

        cont++;
    }
    
    return data->getTrips();
} 

#endif