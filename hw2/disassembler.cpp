#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;
using std::cerr;

/* Hex digits */
char hex2asc[17] = "0123456789ABCDEF";

/* List of instructions */
string halt = "HLT";
string instructions[] = {
    "EXT","LDA","LDI","STA","STI","ADD","SUB","JMP","JMZ","AND","IOR","XOR","ADL",
    "ADC","SBB","NEG","COM","CLR","SET","RTL","RTR","LSL","LSR","ASR","TST","CLC",
    "SEC","TCA","TVA","JAL","NOP"};

/* Convert address to hex */
string convertToHex(int ad){
    string hex = "";
    hex += hex2asc[ad%16];
    ad /= 16;
    hex = hex2asc[ad%16] + hex;
    hex = "0x" + hex;
    return hex;
}

/* Disassemble one byte of instruction and print to screen */
void disassemble(unsigned char instr){
    if(instr == 0){
        cout << halt;
    }else if(instr >> 4 < 15){
        cout << instructions[instr >> 4] + " ";
        cout << "0x" << hex2asc[instr & 15];
        return;
    }else{
        cout << instructions[instr-225];
    }
}


int main(int argc, char **argv){
    unsigned char scram[256];
    int nbytes = 0;
    if(argc > 2){
        cerr << "Incorrect number of arguments" << endl;
        return 1;
    }else if(argc == 2){
        char* fileName = argv[1];
        std::ifstream file(fileName);
        if(file){
	  file.read((char*)scram, 256);
	  nbytes = file.gcount();
        } else{
	  cerr << "Cannot open file: " << fileName << endl;
	  return 2;
        }
    }else {
        nbytes = read(0, scram, 256*sizeof(char));
    }

    // Parse each instruction and print to screen
    for(int i = 0; i < nbytes; i++){
        cout << convertToHex((unsigned char)i) << "  ";
        disassemble(scram[i]);
        cout << endl;
    }
    return (0);
}
