#ifndef TRIP_H
#define TRIP_h

#include <forward_list>

using namespace std;

class Trip{
    private:
        std::vector<Node*> nodes;
        float td;
        float currentLength;
    public:
        Trip(float td)         {this->td = td; this->currentLength = 0 ;};
        ~Trip()     {};
        float getTd()          {return this->td;};
        float getCurrentLength() {return this->currentLength;};
        std::vector<Node*> getNodes()       {return this->nodes;};
        void add(Node *no)     {nodes.push_back(no);};
        Node* front()          {return nodes.front();};
        Node* back()           {
            return nodes.back();
        };
        void print(){
            
            cout << "Dados da trip" << endl;
            cout << "Tamanho total: " << td << endl;
            cout << "Tamanho atual: " << currentLength << endl;
            for (int i = 0; i < nodes.size(); i++){
                cout <<"Nó da trip: "<< nodes[i]->id << " Score: " << nodes[i]->score << endl;
            }
            cout << "-------------------------------------------------------------\n";
        };
        void updateCurrentLength(float val) {this->currentLength += val;};
};

#endif