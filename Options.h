#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>
#include <iostream>
#include <getopt.h>
#include <thread>
#include <err.h>
#include <stdlib.h>
#include "checkpoint.h"
#include "utils.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

// All options are stored as strings.  The only problem this causes is for
// ncores and restart.  Consistency is worth the minor problem.

struct Option {
    std::string name;
    std::string description;
    bool mandatory; // means the option must have a value.
    std::string value;
    bool (*checksanity)(std::string);
};

class Options {
    unsigned int ncores = std::thread::hardware_concurrency();
    bool restart = false;
    std::string checkpointdir = "./metrictest.checkpoint";
    std::string checkpointfname = "options.checkpoint";
    std::string distmatfname = "";
    std::string fastafile = "";
    std::string metricname = "";
    std::string submetricname = "";
    std::string metricopts = "";
    std::string progname = "";

    //void print_usage(void);
    //void initialize_map(void);
    //void processopts(int argc, char **argv);

    public:
        Options(int argc, char **argv);

	void set(const char *key, const char *value) {
	    set(std::string(key), std::string(value));
	};
	void set(const std::string key, const std::string value);

	std::string get(const char *key) const {return get(std::string(key));};
	std::string get(const std::string key) const;
	unsigned int get_ncores() const { return ncores; };
	unsigned int get_restart() const { return restart; };

        void checkpoint(void);
	void cleancheckpointdir(void);
	void restore(void);
};

#endif // OPTIONS_H
