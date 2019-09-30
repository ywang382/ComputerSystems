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
#include <sstream>
#include <stdlib.h>
#include <map>
#include <set>
#include <vector>

#include "assembler.h"

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::map;
using std::ifstream;
using std::stringstream;
using std::set;
using std::vector;

#define EXECUTE_ERROR 4
#define READ_OUT_OF_BOUND 5
#define WRITE_OUT_OF_BOUND 6

/* Hexadecimal characters */
string hex2asc = "0123456789ABCDEF";

/* List of instructions */
map<string, uint8_t> instr = {{"HLT",0},{"EXT",0},{"LDA",16},{"LDI",32},{"STA",48},{"STI",64},
    {"ADD",80},{"SUB",96},{"JMP",112},{"JMZ",128},{"AND",144},{"IOR",160},{"XOR",176},{"ADL",192},
    {"ADC",208},{"SBB",224},{"NEG",240},{"COM",241},{"CLR",242},{"SET",243},{"RTL",244},{"RTR",245},
    {"LSL",246},{"LSR",247},{"ASR",248},{"TST",249},{"CLC",250},{"SEC",251},{"TCA",252},{"TVA",253},
    {"JAL",254},{"NOP",255}};

uint8_t scram[256] = {0}; // SCRAM memory spaces
string scramRef[256] = {""};

unsigned int counter = 0;
map<string, int> labelsMap;
vector<string> prevLabels;
set<string> labels;

/* Write final memory contents to output file */
void dump(char* fileName){
    std::ofstream file(fileName);
    if(!file){
        cerr << "Cannot open file to write: " << fileName << endl;
        exit(3);
    }
    file.write((char*)scram, 256);
}

/* Write final memory contents to cout */
void dump(){
    write(1, (char*)scram, 256);
}

/* Parse one line of input file */
void parse(string line, string &label, string &ope, string &para){
    line = line.substr(0, line.find(";")); // eliminate comments
    if(line.empty()){
        return;
    }
    string buffer;
    // No label exists
    if(isspace(line[0])){
        stringstream ss;
        ss << line;
        ss >> ope >> para;
        if(ss >> buffer){
            //cout << "<" << buffer << ">" << endl;
            cerr << "Invalid argument encountered" << endl;
            exit(6);
        }
        return;
    }
    // Label exists
    unsigned long i = line.find(":");
    if(i == 0 || i == string::npos){
        cerr << "Bad label format" << endl;
        exit(7);
    }
    label = line.substr(0, i);
    if(i+1 < line.length() - 1){
        line = line.substr(i+1);
    } else{
        return;
    }
    stringstream ss;
    ss << line;
    ss >> ope >> para >> buffer;
    if(ss >> buffer){
        cerr << "Invalid argument encountered" << endl;
        exit(6);
    }
}

/* Parse the argument of a command
    @param para: input parameter in string format
    @param constr: constraint, true if parameter can only be 4 bits
 */
uint8_t parseArg(string para, bool constr){
    int result = 0;
    stringstream ss;
    // hexadecimal argument
    if(para.length() > 2 && para.substr(0, 2) == "0x"){
        string hex = para.substr(2);
        if(hex.length() > 2){
            cerr << "Invalid argument encountered" << endl;
            exit(6);
        }
        for(unsigned int i = 0; i < hex.length(); i++){
            if(hex2asc.find(hex[i]) == string::npos){
                cerr << "Invalid argument encountered" << endl;
                exit(6);
            }
        }
        ss << std::hex << para;
        ss >> result;
        return (uint8_t) result;
    }
    if(para.length() == 1 && !isdigit(para[0])){
        cerr << "Invalid argument encountered" << endl;
        exit(6);
    }
    for(unsigned int i = 1; i < para.length(); i++){
        if(!isdigit(para[i])){
            cerr << "Invalid argument encountered" << endl;
            exit(6);
        }
    }
    ss << para;
    ss >> result;
    if(constr && (result < -8 || result > 15)){
        cerr << "Invalid argument encountered" << endl;
        exit(6);
    }
    if(result < -128 || result > 255){
        cerr << "Invalid argument encountered" << endl;
        exit(6);
    }
    return (uint8_t)result;
}

/* Check for valid syntax of label */
bool isLabel(string para){
    if(para.empty() || !isalpha(para[0])){
        return false;
    }
    for(unsigned int i = 1; i < para.length(); i++){
        if(!isalpha(para[i]) && !isdigit(para[i]) && para[i] != '_'){
            return false;
        }
    }
    return true;
}

/* Write memory contents, including replacing labels with appropriate values */
void writeMemory(){
    if(!labels.empty()){
        cerr << "There was an unresolved label reference" << endl;
        exit(8);
    }
    for(int i = 0; i < 256; i++){
        if(!scramRef[i].empty()){
            if(scram[i] == 0){
                cerr << "Invalid op-code encountered" << endl;
                exit(5);
            }
            scram[i] += labelsMap[scramRef[i]];
        }
    }
}

/* Assemble one line of scram code */
void assemble(string line){
    string label, ope, para;
    parse(line, label, ope, para);
    // Skip line if empty
    if(label.empty() && ope.empty() && para.empty()){
        return;
    }

    // Line contains no label
    if(label.empty()){
        if(ope == "ORG" && !para.empty()){
            counter = parseArg(para, false);
            return;
        }
        if(counter > 255){
            cerr << "The program is too large" << endl;
            exit(4);
        }
        if(ope == "DAT" && !para.empty()){
            scram[counter] = parseArg(para, false);
        } else if(ope == "HLT" || instr[ope] > 224){ // No parameter needed
            if(!para.empty()){
                cerr << "Invalid argument encountered" << endl;
                exit(6);
            }
            scram[counter] = instr[ope];
        }else if(instr[ope] <= 224 && !para.empty() && isLabel(para)){ // Label as parameter
            scram[counter] = instr[ope];
            scramRef[counter] = para;
            labels.insert(para);
        } else if((ope == "EXT" && parseArg(para, true) != 0) ||
                    (instr[ope] <= 224 && instr[ope] > 0 && !para.empty() && !isLabel(para))){ // Constant as parameter
            uint8_t temp = parseArg(para, true);
            scram[counter] = instr[ope] + (temp & 15);
        } else{
            cerr << "Invalid op-code encountered" << endl;
            exit(5);
        }
        counter++;
    } else if(!isLabel(label)){ // Invalid label format
        cerr << "Bad label format" << endl;
        exit(7);
    } else { // Line contains label
        if(counter > 255){
            cerr << "The program is too large" << endl;
            exit(4);
        }
        if(ope.empty() && para.empty()){ // Empty label line
            labelsMap[label] = counter;
            labels.erase(label);
            prevLabels.push_back(label);
            return;
        }
        if(ope == "ORG" || labelsMap.count(label)){ // duplicate labels or labels having ORG
            cerr << "Bad label format" << endl;
            exit(7);
        }
        while(!prevLabels.empty()){
            string tempLabel = prevLabels.back();
            labelsMap[tempLabel] = counter;
            labels.erase(tempLabel);
            prevLabels.pop_back();
        }
        labelsMap[label] = counter;
        labels.erase(label);
        if(ope == "DAT" && !para.empty()){
            scram[counter] = parseArg(para, false);
        } else if(ope == "HLT" || instr[ope] > 224){ // No parameter needed
            if(!para.empty()){
                cerr << "Invalid argument encountered" << endl;
                exit(6);
            }
            scram[counter] = instr[ope];
        } else if(instr[ope] <= 224 && !para.empty() && isLabel(para)){ // Label as parameter
            if(label == para && ope == "EXT"){
                cerr << "Invalid argument encountered" << endl;
                exit(6);
            }
            scram[counter] = instr[ope];
            scramRef[counter] = para;
            labels.insert(para);
        } else if((ope == "EXT" && parseArg(para, true) != 0) ||
                    (instr[ope] <= 224 && instr[ope] > 0 && !para.empty() && !isLabel(para))){ // Constant as parameter
            uint8_t temp = parseArg(para, true);
            scram[counter] = instr[ope] + (temp & 15);
        } else{
            cerr << "Invalid op-code encountered" << endl;
            exit(5);
        }
        counter++;
    }
}


// Default simulator, reading from stdin
void assembler(){
    string line;
    while(std::cin) {
        std::getline(std::cin, line);
        assemble(line);
    }
    writeMemory();
    dump();
}

// Simulate from input scram memory image file
void assembler(char* inputFile, char* outputFile){
    ifstream file(inputFile);
    if(!file){
        cerr << "Input file cannot be opened" << endl;
        exit(2);
    }
    string line;
    while(std::getline(file, line)){
        assemble(line);
    }
    writeMemory();
    dump(outputFile);
}

// Simulate from input file and write memory contents to output
void assembler(char* inputFile){
    ifstream file(inputFile);
    if(!file){
        cerr << "Input file cannot be opened" << endl;
        exit(2);
    }
    string line;
    while(std::getline(file, line)){
        assemble(line);
    }
    writeMemory();
    dump();
}






