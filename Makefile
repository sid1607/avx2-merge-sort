CXX=g++ -m64 -std=c++0x
CXXFLAGS=-O3 -Wall
AVXFLAGS=-mavx2

all:
	$(CXX) $(CXXFLAGS) $(AVXFLAGS) -o merge_sort test_sort.cpp merge_sort.cpp 

clean:
	rm -rf *.o merge_sort
