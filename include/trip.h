#ifndef TRIP_H
#define TRIP_h

#include <vector>

typedef unsigned int uint;

class Trip
{
private:
    uint id;                   // Índice da trip
    std::vector<Node> nodes; // Nós que pertencem a trip
    int h0, h1;
    float td;                  // Tamanho máximo da trip
    float currentLength;       // Quanto está ocupado atualmente
public:
    Trip(float td)
    {
        this->td = td;
        this->currentLength = 0;
    };
    void setStartHotel(int h0)    {this->h0 = h0;};
    void setEndHotel(int h1)      {this->h1 = h1;};
    ~Trip(){};
    float getTd() { return this->td; };
    float getCurrentLength() { return this->currentLength; };
    uint getId() { return this->id; };
    std::vector<Node> getNodes() { return this->nodes; };
    Node getNode(int i) { return this->nodes[i]; };
    void add(Node no) { nodes.push_back(no); };
    void add(Node no, int pos) { nodes.insert(nodes.begin() + pos, no); };
    Node front() { return nodes.front(); };
    Node back() { return nodes.back(); };
    void dadosTrip(){
        std::cout << "Tamanho total: " << this->td << std::endl;
        std::cout << "Tamanho atual: " << this->currentLength << std::endl;
    }
    void dadosNodes()
    {
        for (int i = 0; i < nodes.size(); i++)
        {
            std::cout << "Id do nó: "<< nodes[i].id;
            std::cout << "\tX do nó: " << nodes[i].x << "\tY do nó: " << nodes[i].y << "\tScore: " << nodes[i].score << std::endl;
        }
        std::cout << "-------------------------------------------------------------\n";
    };
    void updateCurrentLength(float val1, float val2){
        this->currentLength += val1 + val2;
    };
    void updateCurrentLength(float val) { this->currentLength += val; };
    int getStartHotel() {return this->h0;};
    int getEndHotel()   {return this->h1;};
    int getScoreTrip() {
        int totalScore = 0;
        for(int i = 0; i < nodes.size(); i++){
            totalScore += nodes[i].score;
        }
        return totalScore;
    }
};

#endif