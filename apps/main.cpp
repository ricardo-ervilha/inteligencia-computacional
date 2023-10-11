#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"

using namespace std;

int main(){
    OPHS *data = read_input("../instances/OPHS_instances_February 2013/SET1 1-2/64-45-1-2.ophs");

    pre_processing(data);

    return 0;
}