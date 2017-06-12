// Metric function comparison program

#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>
#include <err.h>

#include "fasta.h"
#include "metric.h"
#include "editmetric.h"
#include "processopts.h"
#include "createmetric.h"
#include "distancematrix.h"

typedef std::vector<std::vector<long double>> distmatrix_t;

std::string progname;
std::string fastafile;
std::string metricname;
std::string submetricname = ""; // might never be set
std::string metricopts;

void checksanity(const distmatrix_t& d);

int
main (int argc, char **argv)
{
    struct rusage startusage, endusage;
    progname = argv[0];

    processopts(argc, argv);
    fastavec_t sequences = readfastafile(fastafile);

    metric *m = createmetric(metricname, submetricname, metricopts, sequences);

    if (getrusage(RUSAGE_SELF, &startusage) < 0) 
        err(1, "getrusage start failed");

    //boost::numeric::ublas::triangular_matrix<double, boost::numeric::ublas::upper> distance(sequences.size(), sequences.size());
    distmatrix_t distance;
    distance.resize(sequences.size());

    for (unsigned int i=0; i<sequences.size(); ++i) {
        distance[i].resize(sequences.size());

	//unused part of the matrix
	for (unsigned int j=0; j<i; ++j)
	    distance[i][j] = -1.0;

	for (unsigned int j=i; j<sequences.size(); ++j) {
	    distance[i][j] = m->compare(sequences[i], sequences[j]);
	}
    }
    if (getrusage(RUSAGE_SELF, &endusage) < 0) 
        err(1, "getrusage start failed");
    
    long usec = endusage.ru_utime.tv_sec - startusage.ru_utime.tv_sec;
    long uusec = endusage.ru_utime.tv_usec - startusage.ru_utime.tv_usec;
    long ssec = endusage.ru_stime.tv_sec - startusage.ru_stime.tv_sec;
    long susec = endusage.ru_stime.tv_usec - startusage.ru_stime.tv_usec;
    std::cerr << (double) usec + uusec / 1000000.0 << " + " 
              << (double) ssec + susec / 1000000.0 << " u+s secs" << std::endl;
    std::cerr << endusage.ru_maxrss - startusage.ru_maxrss << " Kib" << std::endl;

    m->printdetails();

    checksanity(distance);

    // print separately to avoid timing the I/O
    std::cout << "[" << sequences.size() << "," << sequences.size() << "]" << std::endl;
    std::cout << "(";
    for (unsigned int i=0; i<sequences.size(); ++i) {
	if (i != 0) std::cout << "," << std::endl;
        std::cout << "(";
	for (unsigned int j=0; j<sequences.size(); ++j) {
	    if (j != 0) std::cout << ",";
	    std::cout << distance[i][j];
	}
	std::cout << ")";
    }
    std::cout << ")" << std::endl;

    return 0;
}

void
checksanity(const distmatrix_t& d)
{
    int n;

    // Check for non-0 diagonal
    n = 0;
    for (unsigned int i=0; i<d.size(); ++i) 
        if (d[i][i] != 0.0) {
	    ++n;
	    std::cerr << i << " self distance " << d[i][i] << std::endl;
	}
    if (n > 0)
        std::cerr << n << " non-zero self distances" << std::endl;

    // Check upper triangle for 0 not on diagonal.
    n = 0;
    for (unsigned int i=0; i<d.size()-1; ++i) 
	for (unsigned int j=i+1; j<d.size(); ++j) 
	    if (d[i][j] == 0.0) {
		++n;
		std::cerr << i << ", " << j << " distance 0" << std::endl;
	    }

    if (n > 0)
        std::cerr << n << " zero non-self distances" << std::endl;
}
