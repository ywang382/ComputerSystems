CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g

asm: main.o assembler.o assembler.h
	$(CXX) main.o assembler.o -o asm
 
main.o: main.cpp assembler.cpp assembler.h
	$(CXX) $(CXXFLAGS) -c main.cpp
 
simulator.o: assembler.cpp
	$(CXX) $(CXXFLAGS) -c assembler.cpp

clean:
	rm *.o asm
