/*
 Trenton Yuntian Wang
 ywang382@jhu.edu
 CSF spring 2019
*/

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "simulator.h"

using std::string;
using std::cout;
using std::endl;
using std::cerr;

#define EXECUTE_ERROR 4
#define READ_OUT_OF_BOUND 5
#define WRITE_OUT_OF_BOUND 6

/* Hexadecimal characters */
char hex2asc[17] = "0123456789ABCDEF";

/* List of instructions */
string halt = "HLT";
string instructions[] = {
    "EXT","LDA","LDI","STA","STI","ADD","SUB","JMP","JMZ","AND","IOR","XOR","ADL",
    "ADC","SBB","NEG","COM","CLR","SET","RTL","RTR","LSL","LSR","ASR","TST","CLC",
    "SEC","TCA","TVA","JAL","NOP"};

unsigned int pc; // program counter
uint8_t acc; // accumulator
uint8_t mar; // memory address register
bool carry; // carry flag
uint8_t ext = 0; // value stored in EXT operation

uint8_t scram[256]; // SCRAM memory space
unsigned int nbytes; // number of bytes given

/* Load instructions from input file */
void load(char* fileName){
    std::ifstream file(fileName);
    if(!file){
        cerr << "Cannot open file to read: " << fileName << endl;
        exit(2);
    }
    file.read((char*)scram, 256);
    nbytes = file.gcount();
}

/* Write final memory contents to output file */
void dump(char* fileName){
    std::ofstream file(fileName);
    if(!file){
        cerr << "Cannot open file to write: " << fileName << endl;
        exit(3);
    }
    file.write((char*)scram, nbytes);
}

/* Convert address to hex */
string convertToHex(uint8_t ad){
    string hex = "";
    hex += hex2asc[ad%16];
    ad /= 16;
    hex = hex2asc[ad%16] + hex;
    hex = "0x" + hex;
    return hex;
}

/* Check if a given address is out of bound, catches appropriate errors */
void check(uint8_t adr, int error){
    if(adr > nbytes - 1){
        if(error == READ_OUT_OF_BOUND){
            cerr << "Attempting to read data beyond end of file" << endl;
            exit(READ_OUT_OF_BOUND);
        } else if(error == WRITE_OUT_OF_BOUND){
            cerr << "Attempting to write data beyond end of file" << endl;
            exit(WRITE_OUT_OF_BOUND);
        } else {
	    cerr << "Attempting to execute code beyound end of file" << endl;
	    exit(EXECUTE_ERROR);
	}
    }
}

/* Simulate scram programs */
void simulate(){
    for(pc = 0; pc < nbytes; pc++){
        cout << convertToHex(pc) + "  ";
        uint8_t instr = scram[pc]; // current instruction
        uint8_t ope = instr >> 4; // operational instruction (has parameter)
        uint8_t para = instr & 15; // parameter
        uint8_t paraExt = para + ext; // parameter extended by previous EXT
        uint8_t logic = instr - 225; // logical instruction
        
        // Terminate program
        if(instr == 0){
            cout << halt << "     " << "    ACC=" << convertToHex(acc) << endl;
            return;
        }

        if(instructions[ope] == "EXT"){
            ext = para << 4;
	    paraExt = para;
        }else if(instructions[ope] == "LDA"){
            check(paraExt, READ_OUT_OF_BOUND);
            acc = scram[paraExt];
        }else if(instructions[ope] == "LDI"){
            check(paraExt, READ_OUT_OF_BOUND);
	    mar = scram[paraExt];
	    check(mar, READ_OUT_OF_BOUND);
	    acc = scram[mar];
        }else if(instructions[ope] == "STA"){
            check(paraExt, WRITE_OUT_OF_BOUND);
            scram[paraExt] = acc;
        }else if(instructions[ope] == "STI"){
            check(paraExt, WRITE_OUT_OF_BOUND);
	    mar = scram[paraExt];
	    check(mar, WRITE_OUT_OF_BOUND);
	    scram[mar] = acc;
        }else if(instructions[ope] == "ADD"){
            check(paraExt, READ_OUT_OF_BOUND);
	    int res = acc + scram[paraExt];
	    carry = (res > 255);
	    acc = res & 255;
        }else if(instructions[ope] == "SUB"){
            check(paraExt, READ_OUT_OF_BOUND);
	    int res = acc + ~scram[paraExt] + 1;
	    carry = (res > 255);
	    acc = res & 255;
        }else if(instructions[ope] == "JMP"){
            check(paraExt, EXECUTE_ERROR);
            pc = paraExt;
        }else if(instructions[ope] == "JMZ"){
            check(paraExt, EXECUTE_ERROR);
            pc = (acc == 0) ? paraExt : pc;
        }else if(instructions[ope] == "AND"){
            check(paraExt, READ_OUT_OF_BOUND);
	    acc = acc & scram[paraExt];
        }else if(instructions[ope] == "IOR"){
            check(paraExt, READ_OUT_OF_BOUND);
	    acc = acc | scram[paraExt];
        }else if(instructions[ope] == "XOR"){
            check(paraExt, READ_OUT_OF_BOUND);
	    acc = acc ^ scram[paraExt];
        }else if(instructions[ope] == "ADL"){
            check(para, READ_OUT_OF_BOUND);
	    if(ext == 0){
	       int res = acc + (para >> 3 == 0) ? scram[para] : (240 | scram[para]);
	       carry = (res > 255);
	       acc = res & 255;
            } else {
	       int res = acc + paraExt;
	       carry = (res > 255);
	       acc = res & 255;;
            }
        }else if(instructions[ope] == "ADC"){
            check(paraExt, READ_OUT_OF_BOUND);
            acc += scram[paraExt] + (carry) ? 1 : 0;
        }else if(instructions[ope] == "SBB"){
            check(paraExt, READ_OUT_OF_BOUND);
            acc += ~scram[paraExt] + (carry) ? 1 : 0;
        }else if(instructions[logic] == "NEG"){
            acc = ~acc + 1;
        }else if(instructions[logic] == "COM"){
            acc = ~acc;
        }else if(instructions[logic] == "CLR"){
            acc = 0;
        }else if(instructions[logic] == "SET"){
            acc = 255;
        }else if(instructions[logic] == "RTL"){
            acc = ((acc<<1) | (acc>>7)) & 255;
        }else if(instructions[logic] == "RTR"){
            acc = ((acc<<7) | (acc>>1)) & 255;
        }else if(instructions[logic] == "LSL"){
            acc = (acc<<1) & 254;
        }else if(instructions[logic] == "LSR"){
            acc = (acc>>1) & 127;
        }else if(instructions[logic] == "ASR"){
            acc = ((acc>>1) & 127) | (acc & 128);
        }else if(instructions[logic] == "TST"){
            acc = (acc != 0) ? 1 : acc;
        }else if(instructions[logic] == "CLC"){
            carry = false;
        }else if(instructions[logic] == "SEC"){
            carry = true;
        }else if(instructions[logic] == "TCA"){
            acc = (carry) ? 1 : 0;
        }else if(instructions[logic] == "TVA"){
            int neg = (acc & 64) ? 1 : 0;
	    acc = carry ^ neg;
        }else if(instructions[logic] == "JAL"){
            pc = acc & 255;
	    acc = (pc + 1) & 255;
        }else if(instructions[logic] == "NOP"){
	    // No instruction
        }

        // Reset extension
        if(instructions[ope] != "EXT"){
            ext = 0;
        }

        // Print output
        if(instr >> 4 < 15){
            cout << instructions[instr >> 4] + " ";
            cout << convertToHex(paraExt);
        }else{
            cout << instructions[instr-225] << "     ";
        }
        cout << "    ACC=" << convertToHex(acc);
        cout << endl;
    }
}

// Default simulator, reading from stdin
void simulator(){
    nbytes = read(0, scram, 256*sizeof(char));
    simulate();
}

// Simulate from input scram memory image file
void simulator(char* inputFile){
    load(inputFile);
    simulate();
}

// Simulate from input file and write memory contents to output
void simulator(char* inputFile, char* outputFile){
    load(inputFile);
    simulate();
    dump(outputFile);
}






