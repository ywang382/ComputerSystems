CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g

cache: cache.o
	$(CXX) cache.o -o cache

cache.o: cache.cpp
	$(CXX) $(CXXFLAGS) -c cache.cpp

clean:
	rm *.o cache
