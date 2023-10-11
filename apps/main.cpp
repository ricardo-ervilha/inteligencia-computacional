#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"

using namespace std;

int main(){
    OPHS *data = read_input("../instances/OPHS_instances_February 2013/SET5 15-10/100-240-15-10.ophs");

    
    pre_processing(data);

    return 0;
}