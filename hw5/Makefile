CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g

dis: branch.o
	$(CXX) branch.o -o branch

branch.o: branch.cpp
	$(CXX) $(CXXFLAGS) -c branch.cpp

clean:
	rm *.o branch
