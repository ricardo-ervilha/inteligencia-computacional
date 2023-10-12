#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstring>

using namespace std;

vector<string> separa_linha(string line, char separator = '	')
{
    vector<string> v;
    string s;

    stringstream ss(line);

    while (getline(ss, s, separator))
    {
        v.push_back(s);
    }

    return v;
}

Trip **load_solution(string solutionPath, OPHS *data)
{
    cout << "Abrindo arquivo solução no caminho: " << solutionPath << endl;
    ifstream file(solutionPath);
    string line;
    vector<string> solutionTrip;

    if (file.is_open())
    {
        Trip **trips = new Trip *[data->getNumTrips()];

        for(int i = 0; i < data->getNumTrips(); i++){
            Trip* trip = new Trip(data->getTrips()[i]->getTd()); //Td   = trip length for each trip d
            trips[i] = trip;
        }

        for (int i = 0; getline(file, line); i++)
        {
            solutionTrip = separa_linha(line, ' ');

            trips[i]->setStartHotel(stoi(solutionTrip[0]));
            trips[i]->setEndHotel(stoi(solutionTrip[solutionTrip.size() - 1]));

            for (int j = 1; j < solutionTrip.size()-1; j++)
            {
                trips[i]->add(data->getVertex(stoi(solutionTrip[j])));
            }
        }

        for(int i = 0; i < data->getNumTrips(); i++){
            trips[i]->dadosTrip();
            trips[i]->dadosNodes();
        }

        return trips;
    }else{
        cout <<"Erro ao abrir arquivo "<<solutionPath<<endl;
    }

    return nullptr;
}

#endif