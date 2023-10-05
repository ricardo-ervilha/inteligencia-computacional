#include <iostream>
#include "../include/file_io.h"
#include "../src/gdsa_vector.cpp"

using namespace std;

int main(){
    OPHS *data = read_input("../instances/OPHS_instances_February 2013/SET5 15-10/100-240-15-10.ophs");

    cout << data->getNumVertices() << endl;

    //Teste do GDSA
    // GdsaVector<int> v(10);
    // v.pushBack(15);
    // v.pushBack(12);
    // v[0] = -5;
    // cout << v[0] << endl;
    return 0;
}