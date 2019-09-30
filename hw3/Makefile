CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g

sim: main.o simulator.o simulator.h
	$(CXX) main.o simulator.o -o sim
 
main.o: main.cpp simulator.cpp simulator.h
	$(CXX) $(CXXFLAGS) -c main.cpp
 
simulator.o: simulator.cpp
	$(CXX) $(CXXFLAGS) -c simulator.cpp

clean:
	rm *.o sim
