#ifndef _SIMULATOR_H
#define _SIMULATOR_H

void load(char* fileName);
void dump(char* fileName);
void check(uint8_t adr, int error);
void simulate();
void simulator();
void simulator(char* input);
void simulator(char* input, char* output);

#endif
