#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"

using namespace std;

int main(){
    OPHS *data = read_input("../instances/OPHS_instances_February 2013/SET1 3-4/66-130-3-4.ophs");

    
    constructive_algorithm(data);

    return 0;
}