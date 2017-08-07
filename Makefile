CXX=g++
INC=-Iedit_distance/include 
#CXXFLAGS=-Wall -pg -g $(INC)
CXXFLAGS=-Wall -O3 -g $(INC)
LDFLAGS=-lm -pthread -lboost_program_options -lboost_filesystem -lboost_system


OBJS = fasta.o Options.o metric.o editmetric.o kmermetric.o createmetric.o\
	metrictest.o distancematrix.o
metrictest: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS) 

Options.o: Options.cpp Options.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

kmermetric.o: kmermetric.cpp kmermetric.h metric.h
	$(CXX) $(CXXFLAGS) -c -o $@ kmermetric.cpp

editmetric.o: editmetric.cpp editmetric.h metric.h
	$(CXX) $(CXXFLAGS) -c -Wno-sign-compare -o $@ editmetric.cpp

createmetric.o: createmetric.cpp createmetric.h kmermetric.h editmetric.h metric.h

testdistance: testdistance.o distancematrix.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ testdistance.o distancematrix.o

.PHONY: clean
clean:
	rm -f $(OBJS) metrictest

# ncbi toolkit; too complex
#INC=-I/home/ingham/bioinformatics/ncbi_cxx--18_0_0/include\
#    -I/home/ingham/bioinformatics/ncbi_cxx--18_0_0/GCC600-DebugMT64/inc
#LDFLAGS=-L/home/ingham/bioinformatics/ncbi_cxx--18_0_0/local/ncbi-vdb-2.8.0/lib64/\
#        -L/home/ingham/bioinformatics/ncbi_cxx--18_0_0/GCC600-DebugMT64/lib\
#        -lxalgoblastdbindex
