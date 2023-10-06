#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"

using namespace std;

int main(){
    OPHS *data = read_input("../instances/OPHS_instances_February 2013/SET1 1-2/64-45-1-2.ophs");

    Trip** tour = greedy_algorithm(data);

    for(int i = 0; i < data->getNumTrips(); i++){
        cout <<"Trip: " << i << endl;
        cout <<"Td: "<<tour[i]->getTd()<<endl;
        cout <<"currentLength: "<<tour[i]->getCurrentLength()<<endl;
        cout <<"Front: "<<tour[i]->getNodes().front()->id<<endl;        
        
        for (std::forward_list<Node *>::iterator it = tour[i]->getNodes().begin(); it != tour[i]->getNodes().end(); ++it){
            cout <<"Nó da trip: "<<i;
            cout << (*it)->id << endl;
        }
    }

    return 0;
}