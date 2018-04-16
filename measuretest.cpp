// Metric function comparison program

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sys/time.h>
#include <sys/resource.h>
#include <err.h>
#include <exception>

#include "FastaRecord.h"
#include "measure.h"
#include "editmeasure.h"
#include "kmermeasure.h"
#include "cosinemeasure.h"
#include "euclideanmeasure.h"
#include "Options.h"
#include "distancematrix.h"
#include "utils.h"
#include "checkpoint.h"

//#define SINGLETHREAD // single threaded for performance analysis

measure *
createmeasure(const Options opts, const fastavec_t& seqs)
//createmeasure(const std::string& name, const std::string& subname, const std::string& opts, const fastavec_t& seqs)
{
    if (opts.get("measure").compare("edit") == 0) {
        return new editmeasure(opts.get("measureopt"));
    }
    if (opts.get("measure").compare("kmer") == 0) {
        measure *m = nullptr;
        if (opts.get("submeasure").length() > 0) {
            if (opts.get("submeasure").compare("cosine") == 0)
                m = new cosinemeasure(opts.get("measureopt"));
            else if (opts.get("submeasure").compare("cosine") == 0)
                m = new cosinemeasure(opts.get("measureopt"));
        }
// commented out because kmermeasure is a partially abstract class that needs to be subclassed to be used.
//         } else
//             m = new kmermeasure(opts.get("measureopt"));
        if (m != nullptr) {
            m->init(seqs);
            return m;
        }
    }

    // If still here, then the measure is unknown
    std::cerr << "Unknown measure '" << opts.get("measurename") << "'" << std::endl;
    std::cerr << "known measures are: " << "edit, kmer" << std::endl;
    exit(1);
}

void
worker(measure *m, distancematrix *distance, const fastavec_t &sequences,
       unsigned int nthreads, unsigned int workernum, std::string checkpointdir,
       bool restart)
{
    unsigned int startrow;

    if (restart) {
        startrow = workerrestore(workernum, checkpointdir) + nthreads;
    } else {
        startrow = workernum;
    }

    // each worker does rows where row % nthreads == workernum
    // no barrier needed because each worker writes to different locations.
    for (unsigned int i=startrow; i<sequences.size(); i = i + nthreads) {
        for (unsigned int j=i; j<sequences.size(); ++j) {
            distance->set(i, j, m->compare(sequences[i], sequences[j]));
        }
        workercheckpoint(i, workernum, checkpointdir);
    }
}

int
main (int argc, char **argv)
{
    struct rusage startusage, endusage;
    unsigned int nthreads;

    Options opts(argc, argv);
    bool restart = opts.get("restart").compare("true") == 0;

    if (!restart) {
        opts.cleancheckpointdir();
        opts.checkpoint();
    }

    nthreads = opts.get_ncores();
#ifndef SINGLETHREAD
    std::thread threads[nthreads];
#endif

    //!@todo Would it add anything to checkpoint the fasta data structure?
    fastavec_t sequences = readfastafile(opts.get("fasta"));

    //!@todo Would it add anything to checkpoint the metric data structure?
    measure *m = createmeasure(opts, sequences);

    //!@todo assumption: if we are restarting, the checkpoint fasta, metric,
    // are correct for the matrix

    if (getrusage(RUSAGE_SELF, &startusage) < 0)
        err(1, "getrusage start failed");

    distancematrix distance;
    if (opts.get("restart").compare("true") == 0)
        distance.init(opts.get("distmatfname"));
    else
        distance.init(sequences.size(), opts.get("distmatfname"));

#ifdef SINGLETHREAD
    worker(m, &distance, sequences, nthreads, 0, opts.get("checkpointdir"), restart);
#else
    for (unsigned int i=0; i < nthreads; ++i) {
        threads[i] = std::thread(worker, m, &distance, sequences, nthreads, i,
                                 opts.get("checkpointdir"), restart);
    }
    for (unsigned int i=0; i < nthreads; ++i) {
        threads[i].join();
    }
#endif

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

    distance.checksanity();

    if (opts.get("printresult").compare("true") == 0)
        distance.print();

    return 0;
}

