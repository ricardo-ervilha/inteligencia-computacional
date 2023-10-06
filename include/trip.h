#ifndef TRIP_H
#define TRIP_h

#include <forward_list>

class Trip{
    private:
        std::forward_list<Node*> nodes;
        float td;
        float currentLength;
    public:
        Trip(float td)         {this->td = td; this->currentLength = 0 ;};
        ~Trip()     {};
        float getTd()          {return this->td;};
        float getCurrentLength() {return this->currentLength;};
        std::forward_list<Node*> getNodes()       {return this->nodes;};
        void add(Node *no)     {nodes.push_front(no);};
        Node* front()          {return nodes.front();};
        void updateCurrentLength(float val) {this->currentLength += val;};
};

#endif