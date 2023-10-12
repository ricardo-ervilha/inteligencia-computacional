#ifndef FILE_IO_H
#define FILE_IO_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>
#include "OPHS.h"

using namespace std;

vector<string> separa_linha(string line){
    vector<string> v;
    string s;

    stringstream ss(line);

    while (getline(ss, s, '	')) {
        v.push_back(s);
    }

    return v;
} 

OPHS* read_input(string path)
{
    ifstream file(path);

    string line;
    int numVertices, numExtraHotels, numTrips;
    int tMax;
    vector<string> v;

    OPHS* data;

    if(file.is_open()){

        getline(file, line);
        v = separa_linha(line);
        numVertices = stoi(v[0]); //Número de vértices + 2 
        
        numExtraHotels = stoi(v[1]); //Número de hoteis extras
        numTrips = stoi(v[2]); //Número de trips

        getline(file, line);
        tMax = stoi(line); //Tamanho máximo do tour

        getline(file, line);
        v = separa_linha(line);

        Trip **trips = new Trip*[numTrips];
        for(int i = 0; i < numTrips; i++){
            Trip* trip = new Trip(stof(v[i])); //Tamanho de cada trip
            trips[i] = trip;
        }

        getline(file, line);

        Node *nos = new Node[numVertices + numExtraHotels];

        for(int i = 0; i < numExtraHotels + 2; i++){
            //Computa tanto os hoteis de início e fim quanto os hoteis extras
            getline(file,line);
            v = separa_linha(line);
            nos[i].x = stof(v[0]); //coordenada x do hotel i
            nos[i].y = stof(v[1]); //coordenada y do hotel i
            nos[i].score = stoi(v[2]); //score do hotel i (problema implica ser zero)
            nos[i].id = i;
            nos[i].vis = false;
        }

        for(int i = numExtraHotels + 2; i < numExtraHotels + numVertices; i++){

            getline(file, line);
            v = separa_linha(line);
            nos[i].x = stof(v[0]); //coordenada x do no i
            nos[i].y = stof(v[1]); //coordenada y do no i
            nos[i].score = stoi(v[2]); //score do hotel i
            nos[i].vis = false;
            nos[i].id = i;
        }



        float **d_matrix = new float*[numVertices + numExtraHotels];

        for(int i = 0; i < numVertices + numExtraHotels; i++)
            d_matrix[i] = new float[numVertices + numExtraHotels];
        create_distance_matrix(d_matrix, numVertices + numExtraHotels, nos);
        // print_distance_matrix(d_matrix, numVertices + numExtraHotels);
        data = new OPHS(numVertices, numExtraHotels, numTrips, tMax, nos, d_matrix, trips);


    }

    return data;
}

#endif