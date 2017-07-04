#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>
#include <iostream>
#include <getopt.h>
#include <thread>
#include <err.h>
#include <stdlib.h>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

#include "single_option.h"

typedef std::map<std::string, std::string> optmap;


class Options {
    unsigned int nthreads = 1;
    bool restart = false;
    std::string null = "";
    std::string default_checkpointdir = "./metrictest.checkpoint";
    std::string checkpointfname = "options.checkpoint";
    optmap strvalues;

    single_option options[9] = {
        { std::string("fasta file containing sequences; required"),
	  std::string("fasta"), 'f', required_argument, std::string("") },
        { std::string("distance measure to use; required"),
	  std::string("metric"), 'm', required_argument, std::string("") },
        { std::string("submetric to use; optional; depends on the distance measure"),
	  std::string("submetric"), 's', required_argument, std::string("") },
        { std::string("options for the distance measure; depends on the measure"),
	  std::string("metricopt"), 'o', required_argument, std::string("") },
        { std::string("distance matrix file name; required"),
	  std::string("distmatfname"), 'd', required_argument, std::string("") },
        { std::string("number of CPU cores to use; optional; default: 1"),
	  std::string("ncores"), 't', required_argument, std::string("1") },
        { std::string("checkpoint directory; required"),
	  std::string("checkpoint"), 'c', required_argument, std::string("./metrictest.checkpoint") },
        { std::string("restart from checkpoint; optional; default: not restarting from checkpoing"),
	  std::string("restart"), 'r', no_argument, std::string("false") }
        { std::string("program name; never set this"),
	  std::string("progname"), 'p', required_argument, std::string("") }
    };

    void print_usage(void);
    void initialize_map(void);
    void processopts(int argc, char **argv);

    public:
        Options(int argc, char **argv) {
	    std::cerr << "Constructor\n";
	    //initialize_map();
	    processopts(argc, argv);
	};

	void set(const std::string key, const std::string value);

	std::string get(const char *key) const {return get(std::string(key));};
	std::string get(const std::string key) const {
	    if (strvalues.find(key) == strvalues.end())
	        warn("No option entry with key '%s'", key.c_str());
	    return strvalues.at(key);
	};
	unsigned int get_nthreads() const { return nthreads; };
	unsigned int get_restart() const { return restart; };

        void checkpoint(void);
};

#endif // OPTIONS_H
