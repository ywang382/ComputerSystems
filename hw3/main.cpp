/*
 Trenton Yuntian Wang
 ywang382@jhu.edu
 CSF spring 2019
*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#include "simulator.h"

using std::string;
using std::cout;
using std::endl;
using std::cerr;


int main(int argc, char** argv){
    if(argc > 3){
        cerr << "Incorrect number of arguments" << endl;
        exit(1);
    } else if(argc == 3){
        char* inputFile = argv[1];
        char* outputFile = argv[2];
        simulator(inputFile, outputFile);
    } else if(argc == 2){
        char* inputFile = argv[1];
        simulator(inputFile);
    } else{
        simulator();
    }
    return 0;
}



