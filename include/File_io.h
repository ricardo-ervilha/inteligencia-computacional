#ifndef FILE_IO_H
#define FILE_IO_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>
#include "Node.h"

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

void read_input(string path)
{
    ifstream file(path);

    string line;
    int numVertices, numExtraHotels, numTrips;
    int tMax;
    vector<string> v;
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
        float *tds = new float[numTrips];
        for(int i = 0; i < numTrips; i++){
            tds[i] = stof(v[i]); //Tamanho de cada trip
        }

        getline(file, line);

        Node *hoteis = new Node[numExtraHotels + 2];

        for(int i = 0; i < numExtraHotels+2; i++){
            //Computa tanto os hoteis de início e fim quanto os hoteis extras
            getline(file,line);
            v = separa_linha(line);
            hoteis[i].x = stof(v[0]); //coordenada x do hotel i
            hoteis[i].y = stof(v[1]); //coordenada y do hotel i
            hoteis[i].score = stoi(v[2]); //score do hotel i (problema implica ser zero)
        } 

        Node *nos = new Node[numVertices - 2];
        for(int i = 0; i < numVertices-2; i++){
            getline(file, line);
            v = separa_linha(line);
            nos[i].x = stof(v[0]); //coordenada x do no i
            nos[i].y = stof(v[1]); //coordenada y do no i
            nos[i].score = stoi(v[2]); //score do hotel i
        }

    }
}

#endif