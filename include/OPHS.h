#ifndef OPHS_H
#define OPHS_H

#include "Node.h"

//Classe para condensar todos os dados do problema

class OPHS{
private:
    int numVertices;
    int numExtraHotels;
    int numTrips;
    float totalTourLength;
    float* tds; //trip length for each trip d
    Node *hotels;
    Node *vertex;
public:
    OPHS(int numVertices, int numExtraHotels, int numTrips, float totalTourLength, float *tds, Node* hotels, Node* vertex){
        this->numVertices = numVertices;
        this->numExtraHotels = numExtraHotels;
        this->numTrips;
        this->totalTourLength = totalTourLength;
        this->tds = tds;
        this->hotels = hotels;
        this->vertex = vertex;
    };

    ~OPHS(){
        delete [] tds;
        delete [] hotels;
        delete [] vertex;
    };

    int getNumVertices()     {return this->numVertices;};
    int getNumExtraHotels()  {return this->numExtraHotels;};
    int getNumTrips()        {return this->numTrips;};
    float getTotalLength()   {return this->totalTourLength;};
    Node* getVertex()        {return this->vertex;};
    Node* getHotels()        {return this->hotels;};
};

#endif