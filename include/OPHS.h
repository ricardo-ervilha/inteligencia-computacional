#ifndef OPHS_H
#define OPHS_H

#include "Node.h"
#include "trip.h"

//Classe para condensar todos os dados do problema

class OPHS{
private:
    int numVertices;
    int numExtraHotels;
    int numTrips;
    float totalTourLength;
    Trip **trips; //Guarda as trips do tour
    Node *vertex; //Guarda os hoteis e nós
    float **d_matrix; //Matriz com distâncias
    
public:
    OPHS(int numVertices, int numExtraHotels, int numTrips, float totalTourLength, Node* vertex, float **d_matrix, Trip **trips){
        this->numVertices = numVertices;
        this->numExtraHotels = numExtraHotels;
        this->numTrips = numTrips;
        this->totalTourLength = totalTourLength;
        this->vertex = vertex;
        this->d_matrix = d_matrix;
        this->trips = trips;
    };

    ~OPHS(){
        delete [] vertex;
        for(int i = 0; i < getNumVertices() + getNumExtraHotels(); i++){
            delete [] d_matrix[i];
        }
    };

    int getNumVertices()     {return this->numVertices;};
    int getNumExtraHotels()  {return this->numExtraHotels;};
    int getNumTrips()        {return this->numTrips;};
    float getTotalLength()   {return this->totalTourLength;};
    Node* getVertex()        {return this->vertex;};
    Node getVertex(int i)   {return this->vertex[i];};
    float getDistance(int i, int j) {return this->d_matrix[i][j];};
    Trip** getTrips()     {return this->trips;};
    void setTrips(Trip **trips) {this->trips = trips;};
    Trip* getTrip(int i)  {return this->trips[i];};
    float** getd_matrix()     {return this->d_matrix;};
    
    void printDadosOPHS(){
        std::cout << "-=-=-=-=-=-=-=-=-=-=-=-= Dados Gerais -=-=-=-=-=-=-=-=-=-=-=-=\n";
        std::cout << "Número de vértices => "<< this->numVertices << std::endl;
        std::cout << "Número de Extra Hoteis => "<< this->numExtraHotels << std::endl;
        std::cout << "Número de Trips => "<< this->numTrips << std::endl;
        std::cout << "Tamanho total do Tour => "<< this->totalTourLength << std::endl;
        std::cout << "-------------------------------------------------------------\n";
        
        for(int i = 0; i < numTrips; i++){
            cout << "@Trip " << i << endl;
            trips[i]->dadosTrip();
            trips[i]->dadosNodes();
            std::cout << "-------------------------------------------------------------\n";
        }

        cout << endl;
        cout << endl;
        
    }
};

#endif