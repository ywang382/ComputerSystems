CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g

dis: disassembler.o
	$(CXX) disassembler.o -o dis

disassembler.o: disassembler.cpp
	$(CXX) $(CXXFLAGS) -c disassembler.cpp

clean:
	rm *.o dis
