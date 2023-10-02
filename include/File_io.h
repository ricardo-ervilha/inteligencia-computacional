#ifndef FILE_IO_H
#define FILE_IO_H

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void read_input(string path)
{
    ifstream file(path);

    string line;
    string numVertices, numExtraHotels, numTrips;
    string tMax;
    string td;
    
    if(file.is_open()){
        getline(file, line);
        char* p; 
        strcpy(p, line.c_str());
        p = strtok(p, " ");

        std::cout << "Qualquer coisa";
        for (int i = 0; i < 3; i++)
        {
            std::cout << p[i];
        }
    }
}

#endif