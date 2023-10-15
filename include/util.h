#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstring>
#include <random>

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

/**
 *
 * @param min
 * @param max
 * @param gen
 * @return
 */
double doubleRandom(double min, double max, mt19937 *gen)
{
    uniform_real_distribution<double> distributionDouble(min, max); // distribuição uniforme entre min e max
    return distributionDouble(*gen);
}
/**
 *
 * @param min
 * @param max
 * @param gen
 * @return
 */
int intRandom(int min, int max, mt19937 *gen)
{
    uniform_int_distribution<int> distributionInt(min, max); // distribuição uniforme entre min e max
    return distributionInt(*gen);
}

Trip **makeCopySolution(OPHS *data, Trip **initialSolution)
{
   
    Trip **copyTrips = new Trip *[data->getNumTrips()];

    // inicializar as trips
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        
        copyTrips[i] = new Trip(data->getTrips()[i]->getTd()); // Td   = trip max length for each trip d
        
        copyTrips[i]->setStartHotel(initialSolution[i]->getStartHotel());
        
        copyTrips[i]->setEndHotel(initialSolution[i]->getEndHotel());
        
        copyTrips[i]->updateCurrentLength(initialSolution[i]->getCurrentLength());
        
        copyTrips[i]->setNodes(initialSolution[i]->getNodes());
    }

    return copyTrips;
}

float getScoreTour(OPHS *data, Trip **trips)
{
    float totalScore = 0;
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        totalScore += trips[i]->getScoreTrip();
    }

    return totalScore;
}

float calcTripLength(OPHS *data, Trip *trip)
{
    float lengthTrip = 0;
    int idAnterior = trip->getStartHotel();
    // cout << "H0: " << data->getVertex(trip->getStartHotel()).id << " x: " << data->getVertex(trip->getStartHotel()).x << " y: " << data->getVertex(trip->getStartHotel()).y << endl;
    for (int i = 0; i < trip->getNodes().size(); i++)
    {
        lengthTrip += data->getDistance(idAnterior, trip->getNodes()[i].id);
        idAnterior = trip->getNodes()[i].id;
        // cout << "\tID: " << trip->getNodes()[i].id << " x: " << trip->getNodes()[i].x << " y: " << trip->getNodes()[i].y << endl;
    }
    lengthTrip += data->getDistance(idAnterior, trip->getEndHotel());
    // cout << "H1: " << data->getVertex(trip->getEndHotel()).id << " x: " << data->getVertex(trip->getEndHotel()).x << " y: " << data->getVertex(trip->getEndHotel()).y << endl;
    // cout << "Distancia: " << lengthTrip << endl;

    return lengthTrip;
}

float calcTripLength(OPHS *data, int startHotel, int endHotel, vector<Node> nos)
{
    float lengthTrip = 0;
    int idAnterior = startHotel;
    // cout << "H0: " << startHotel << " x: " << data->getVertex(startHotel).x << " y: " << data->getVertex(startHotel).y << endl;
    for (int i = 0; i < nos.size(); i++)
    {
        lengthTrip += data->getDistance(idAnterior, nos[i].id);
        idAnterior = nos[i].id;
        // cout << "\tID: " << nos[i].id << " x: " << nos[i].x << " y: " << nos[i].y << endl;
    }
    lengthTrip += data->getDistance(idAnterior, endHotel);
    // cout << "H1: " << data->getVertex(endHotel).id << " x: " << data->getVertex(endHotel).x << " y: " << data->getVertex(endHotel).y << endl;
    // cout << "Distancia: " << lengthTrip << endl;

    return lengthTrip;
}

float calcTripScore(vector<Node> nodesTrip)
{
    float totalScore = 0;
    for (int i = 0; i < nodesTrip.size(); i++)
    {
        totalScore += nodesTrip[i].score;
    }
    return totalScore;
}

void load_solution(string solutionPath, OPHS *data)
{
    cout << "Abrindo arquivo solução no caminho: " << solutionPath << endl;
    ifstream file(solutionPath);
    string line;
    vector<string> solutionTrip;

    if (file.is_open())
    {
        Trip **trips = new Trip *[data->getNumTrips()];

        // inicializar as trips
        for (int i = 0; i < data->getNumTrips(); i++)
        {
            Trip *trip = new Trip(data->getTrips()[i]->getTd()); // Td   = trip max length for each trip d
            trips[i] = trip;
        }

        for (int i = 0; getline(file, line); i++)
        {
            solutionTrip = separa_linha(line, ' '); // ids dos nos separados por espaço

            trips[i]->setStartHotel(stoi(solutionTrip[0]));                     // primeiro é o hotel de inicio
            trips[i]->setEndHotel(stoi(solutionTrip[solutionTrip.size() - 1])); // ultimo é o hotel final da trip

            for (int j = 1; j < solutionTrip.size() - 1; j++)
            {
                trips[i]->add(data->getVertex(stoi(solutionTrip[j])));
            }

            trips[i]->updateCurrentLength(calcTripLength(data, trips[i]));
        }
        data->setTrips(trips);
    }
    else
    {
        cout << "Erro ao abrir arquivo " << solutionPath << endl;
    }
}

void printTrips(OPHS *data, Trip **trips)
{
    cout << endl;
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        trips[i]->dadosTrip();
        trips[i]->dadosNodes();
    }
    cout << endl;
}

void printIdsTrips(OPHS *data, Trip **trips)
{
    cout << endl;
    for (int i = 0; i < data->getNumTrips(); i++)
    {
        vector<Node> nodes = trips[i]->getNodes();
        for (int j = 0; j < nodes.size(); j++)
        {
            cout << nodes[j].id << " - ";
        }
        cout << endl;
    }
}

#endif