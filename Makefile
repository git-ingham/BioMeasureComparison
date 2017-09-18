BUILDDIR=objs
SRCDIR=.

# C compiler choice
#CXX=g++
CXX=clang++

INC=-Iedit_distance/include
CXXFLAGS=-Wall -g $(INC) -std=c++11
#CXXFLAGS=-Wall -O3 -g $(INC) -std=c++11

# for static linking when the target is a different system missing
# required libraries.  Need to get supercomputer people to update their
# OS.
STATIC=
#STATIC=-lstatic

LDFLAGS=$(STATIC) -lm -pthread -lboost_program_options -lboost_filesystem -lboost_system

default: metrictest README.txt

#OBJS = fasta.o Options.o metric.o editmetric.o kmermetric.o createmetric.o\
#	metrictest.o distancematrix.o utils.o checkpoint.o editcost.o
SRCS = checkpoint.cpp createmetric.cpp distancematrix.cpp editcost.cpp\
	editmetric.cpp fasta.cpp kmermetric.cpp metric.cpp metrictest.cpp\
	Options.cpp utils.cpp
OBJS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SRCS))
metrictest: $(BUILDDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) 

$(BUILDDIR):
	[ -d $(BUILDDIR) ] || mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/checkpoint.o: $(SRCDIR)/checkpoint.cpp $(SRCDIR)/checkpoint.h $(SRCDIR)/Options.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/Options.o: $(SRCDIR)/Options.cpp $(SRCDIR)/Options.h $(SRCDIR)/utils.h $(SRCDIR)/checkpoint.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/kmermetric.o: $(SRCDIR)/kmermetric.cpp $(SRCDIR)/kmermetric.h $(SRCDIR)/metric.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/createmetric.o: $(SRCDIR)/createmetric.cpp $(SRCDIR)/createmetric.h $(SRCDIR)/kmermetric.h $(SRCDIR)/editmetric.h $(SRCDIR)/metric.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/metrictest.o: $(SRCDIR)/metrictest.cpp $(SRCDIR)/utils.h $(SRCDIR)/checkpoint.h $(SRCDIR)/fasta.h $(SRCDIR)/Options.h $(SRCDIR)/editmetric.h $(SRCDIR)/distancematrix.h 
	$(CXX) -c $(CXXFLAGS) -o $@ $<
$(BUILDDIR)/editmetric.o: $(SRCDIR)/editmetric.cpp $(SRCDIR)/editmetric.h $(SRCDIR)/metric.h
	$(CXX) -c $(CXXFLAGS) -Wno-sign-compare -o $@ editmetric.cpp

README.txt: README.md
	pandoc -f markdown -t plain --wrap=none README.md -o README.txt

testdistance: $(SRCDIR)/testdistance.o $(SRCDIR)/distancematrix.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $*

MeasureComparison.tgz: 
	(cd ..; tar czf MeasureComparison/$@ MeasureComparison/{*.cpp,*.h,Makefile,edit_distance})

.PHONY: clean
clean:
	rm -f $(OBJS) metrictest

# ncbi toolkit; too complex, at least for now
#INC=-I/home/ingham/bioinformatics/ncbi_cxx--18_0_0/include\
#    -I/home/ingham/bioinformatics/ncbi_cxx--18_0_0/GCC600-DebugMT64/inc
#LDFLAGS=-L/home/ingham/bioinformatics/ncbi_cxx--18_0_0/local/ncbi-vdb-2.8.0/lib64/\
#        -L/home/ingham/bioinformatics/ncbi_cxx--18_0_0/GCC600-DebugMT64/lib\
#        -lxalgoblastdbindex
