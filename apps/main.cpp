#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"

using namespace std;

int main(){
    OPHS *data = read_input("../instances/OPHS_instances_February 2013/SET4/100-20-3-2.ophs");

    Trip** tour = greedy_algorithm(data);

    for(int i = 0; i < data->getNumTrips(); i++){
        cout << "(------)(------)(------)(------)(------)(------)(------)\n";
        cout << "Índice da Trip: " << i << endl;
        tour[i]->print();
    }

    return 0;
}