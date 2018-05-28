BUILDDIR=objs
SRCDIR=.

# C compiler choice
CXX=g++
#CXX=clang++

INC=-Iedit_distance/include
CXXFLAGS=-Wall -g $(INC) -std=c++17
#CXXFLAGS=-Wall -O3 -g $(INC) -std=c++11

# for static linking when the target is a different system missing
# required libraries.  Need to get supercomputer people to update their
# OS.
STATIC=
#STATIC=-lstatic

#LDFLAGS=$(STATIC) -lm -pthread -lboost_program_options -lboost_filesystem -lboost_system -llog4cplus
LDFLAGS=$(STATIC) -lm -pthread -lboost_program_options -lboost_filesystem -lboost_system -llog4cxx

default: measuretest README.txt

SRCS = checkpoint.cpp distancematrix.cpp editcost.cpp editmeasure.cpp\
	FastaRecord.cpp measuretest.cpp Options.cpp utils.cpp kmerset.cpp\
	deBruijnGraph.cpp cosinemeasure.cpp euclideanmeasure.cpp
OBJS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SRCS))
measuretest: $(BUILDDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) 

$(BUILDDIR):
	[ -d $(BUILDDIR) ] || mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/checkpoint.o: $(SRCDIR)/checkpoint.cpp $(SRCDIR)/checkpoint.h $(SRCDIR)/Options.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/Options.o: $(SRCDIR)/Options.cpp $(SRCDIR)/Options.h $(SRCDIR)/utils.h $(SRCDIR)/checkpoint.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/measuretest.o: $(SRCDIR)/measuretest.cpp $(SRCDIR)/utils.h $(SRCDIR)/checkpoint.h $(SRCDIR)/FastaRecord.h $(SRCDIR)/Options.h $(SRCDIR)/editmeasure.h $(SRCDIR)/distancematrix.h 
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/editmeasure.o: $(SRCDIR)/editmeasure.cpp $(SRCDIR)/editmeasure.h $(SRCDIR)/measure.h
	$(CXX) -c $(CXXFLAGS) -Wno-sign-compare -o $@ editmeasure.cpp
$(BUILDDIR)/deBruijnGraph.o: $(SRCDIR)/deBruijnGraph.cpp $(SRCDIR)/deBruijnNode.h $(SRCDIR)/kmerint.h $(SRCDIR)/intbase.h

README.txt: README.md
	-pandoc -f markdown -t plain --wrap=none README.md -o README.txt

TESTEXE=testdistance testkmerint testdebruijnnode testintbase testdebruijn
TESTOBJS=${TESTEXE}\
	$(BUILDDIR)/testkmerint.o $(BUILDDIR)/testdebruijnnode.o\
	$(BUILDDIR)/testintbase.o $(BUILDDIR)/testdebruijn.o

testdistance: $(BUILDDIR)/testdistance.o $(BUILDDIR)/distancematrix.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $*
$(BUILDDIR)/testdistance.o: testdistance.cpp distancematrix.h

testkmerint: $(BUILDDIR)/testkmerint.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(BUILDDIR)/testkmerint.o
$(BUILDDIR)/testkmerint.o: $(SRCDIR)/testkmerint.cpp $(SRCDIR)/kmerint.h
	$(CXX) -c $(CXXFLAGS) -o $@ testkmerint.cpp

testdebruijnnode: $(BUILDDIR)/testdebruijnnode.o deBruijnNode.h\
	kmerint.h kmer.h intbase.h deBruijnGraph.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(BUILDDIR)/testdebruijnnode.o
$(BUILDDIR)/testdebruijnnode.o: $(SRCDIR)/testdebruijnnode.cpp $(SRCDIR)/deBruijnNode.h
	$(CXX) -c $(CXXFLAGS) -o $@ testdebruijnnode.cpp

$(BUILDDIR)/testintbase.o: testintbase.cpp $(SRCDIR)/intbase.h $(SRCDIR)/intbaseDNA.h $(SRCDIR)/intbase2.h $(SRCDIR)/intbaseOPs.h
	$(CXX) -c $(CXXFLAGS) -o $@ testintbase.cpp
testintbase: $(BUILDDIR)/testintbase.o $(SRCDIR)/intbase.h
	$(CXX) $(CXXFLAGS) -o $@ $(LDFLAGS) $(BUILDDIR)/testintbase.o 

testdebruijn: $(BUILDDIR)/testdebruijn.o $(BUILDDIR)/deBruijnGraph.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(BUILDDIR)/testdebruijn.o $(BUILDDIR)/deBruijnGraph.o
$(BUILDDIR)/testdebruijn.o: $(SRCDIR)/testdebruijn.cpp $(BUILDDIR)/deBruijnGraph.o
	$(CXX) -c $(CXXFLAGS) -o $@ testdebruijn.cpp

all: ${TESTEXE} measuretest

.PHONY: clean
clean:
	rm -f $(OBJS) $(TESTOBJS) measuretest

# ncbi toolkit; too complex, at least for now
#INC=-I/home/ingham/bioinformatics/ncbi_cxx--18_0_0/include\
#    -I/home/ingham/bioinformatics/ncbi_cxx--18_0_0/GCC600-DebugMT64/inc
#LDFLAGS=-L/home/ingham/bioinformatics/ncbi_cxx--18_0_0/local/ncbi-vdb-2.8.0/lib64/\
#        -L/home/ingham/bioinformatics/ncbi_cxx--18_0_0/GCC600-DebugMT64/lib\
#        -lxalgoblastdbindex
