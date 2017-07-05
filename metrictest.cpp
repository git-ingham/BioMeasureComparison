// Metric function comparison program

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>
#include <err.h>

#include "fasta.h"
#include "metric.h"
#include "Options.h"
#include "editmetric.h"
#include "createmetric.h"
#include "distancematrix.h"


void
checksanity(const distancematrix& d)
{
    int n;

    // Check for non-0 diagonal
    n = 0;
    for (unsigned int i=0; i<d.get_size(); ++i) 
        if (d.get(i,i) != 0.0) {
	    ++n;
	    std::cerr << i << " self distance " << d.get(i,i) << std::endl;
	}
    if (n > 0)
        std::cerr << n << " non-zero self distances" << std::endl;

    // Check upper triangle for 0 not on diagonal.
    n = 0;
    for (unsigned int i=0; i<d.get_size()-1; ++i) 
	for (unsigned int j=i+1; j<d.get_size(); ++j) 
	    if (d.get(i,j) == 0.0) {
		++n;
		std::cerr << i << ", " << j << " distance 0" << std::endl;
	    }

    if (n > 0)
        std::cerr << n << " zero non-self distances" << std::endl;
}

void
worker(const metric *m, distancematrix *distance, const fastavec_t &sequences, unsigned int nthreads, unsigned int workernum)
{
    // each worker does rows where row % nthreads == workernum
    // no barrier needed because each worker writes to different locations.
    for (unsigned int i=workernum; i<sequences.size(); i = i + nthreads) {
	for (unsigned int j=i; j<sequences.size(); ++j) {
	    distance->set(i, j, m->compare(sequences[i], sequences[j]));
	}
    }
}

int
main (int argc, char **argv)
{
    struct rusage startusage, endusage;
    std::vector<std::thread> threads;
    unsigned int nthreads;

    Options opts(argc, argv);
    opts.checkpoint();

    nthreads = opts.get_ncores();
    threads.reserve(nthreads);

    fastavec_t sequences = readfastafile(opts.get("fasta"));

    metric *m = createmetric(opts, sequences);

    if (getrusage(RUSAGE_SELF, &startusage) < 0) 
        err(1, "getrusage start failed");

    distancematrix distance(sequences.size(), opts.get("distmatfname"));

    for (unsigned int i=0; i < nthreads; ++i) {
        threads[i] = std::thread(worker, m, &distance, sequences, nthreads, i);
    }
    for (unsigned int i=0; i < nthreads; ++i) {
        threads[i].join();
    }
    if (getrusage(RUSAGE_SELF, &endusage) < 0) 
        err(1, "getrusage end failed");
    
    long usec = endusage.ru_utime.tv_sec - startusage.ru_utime.tv_sec;
    long uusec = endusage.ru_utime.tv_usec - startusage.ru_utime.tv_usec;
    long ssec = endusage.ru_stime.tv_sec - startusage.ru_stime.tv_sec;
    long susec = endusage.ru_stime.tv_usec - startusage.ru_stime.tv_usec;
    std::cerr << "nthreads: " << nthreads << std::endl;
    std::cerr << (double) usec + uusec / 1000000.0 << " + " 
              << (double) ssec + susec / 1000000.0 << " u+s secs" << std::endl;
    std::cerr << endusage.ru_maxrss - startusage.ru_maxrss << " Kib" << std::endl;

    m->printdetails();

    checksanity(distance);

    distance.print();

    return 0;
}

