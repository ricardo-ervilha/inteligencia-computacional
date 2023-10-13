#include <iostream>
#include "../include/file_io.h"
#include "../include/greedy.h"
#include "util.h"

using namespace std;

int main(){
    string instance_folder = "../instances/OPHS_instances_February 2013/";
    string instance_name = "SET1 102/64-45-1-2";

    OPHS *data = read_input(instance_folder+instance_name+".ophs");

    constructive_algorithm(data);

    //Trip** tour = load_solution("../out/"+instance_name+".ophsout", data);

    return 0;
}