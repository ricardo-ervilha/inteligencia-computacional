#include <iostream>
#include "../include/file_io.h"
#include "../src/gdsa_vector.cpp"

using namespace std;

int main(){
    // read_input("../instances/OPHS_instances_February 2013/SET5 15-10/100-240-15-10.ophs");

    GdsaVector<int> *v = new GdsaVector<int>(10);
    v->endAdd(15);
    v->endAdd(12);
    for(int i = 0; i < v->length(); i++){
        cout << v->get(i) << endl;
    }
    return 0;
}