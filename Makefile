CXX=g++ -m64 -std=c++0x
CXXFLAGS=-O3 -Wall
AVXFLAGS=-mavx2

all:
	$(CXX) $(CXXFLAGS) $(AVXFLAGS) -o merge_sort merge_sort.cpp test_sort.cpp gen_sort.cpp

clean:
	rm -rf *.o merge_sort