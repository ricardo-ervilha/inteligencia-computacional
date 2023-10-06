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
    Trip **tour;
    Node *vertex;
    float **d_matrix;
public:
    OPHS(int numVertices, int numExtraHotels, int numTrips, float totalTourLength, Node* vertex, float **d_matrix){
        this->numVertices = numVertices;
        this->numExtraHotels = numExtraHotels;
        this->numTrips = numTrips;
        this->totalTourLength = totalTourLength;
        this->vertex = vertex;
        this->d_matrix = d_matrix;
    };

    ~OPHS(){
        delete [] vertex;
    };

    int getNumVertices()     {return this->numVertices;};
    int getNumExtraHotels()  {return this->numExtraHotels;};
    int getNumTrips()        {return this->numTrips;};
    float getTotalLength()   {return this->totalTourLength;};
    Node* getVertex()        {return this->vertex;};
    float getDistance(int i, int j) {return this->d_matrix[i][j];};
    Trip** getTrips()     {return this->tour;};
};

#endif