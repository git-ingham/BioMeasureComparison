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
#include "measure.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

// All options are stored as strings.  The only problem this causes is for
// ncores and restart.  Hopefully, consistency is worth the minor problem.

struct Option {
    std::string name;
    char shortflag;
    char type; // currently unused, for future type checking
    std::string description;
    bool mandatory; // means the option must have a value.
    bool has_argument;
    std::string value;
    std::string (*checksanity)(const std::string);
};

class Options {
    const static unsigned int nopts = 9;
    struct Option option_defs[nopts] {
	{ "restart", 'r', 'b', "restart from checkpoint; optional; default: not restarting from checkpoint",
	  false, false, "", nullptr },
	{ "fasta", 'f', 's', "fasta file containing sequences; required", true,
	  true, "", nullptr },
	{ "measure", 'm', 's', "distance measure to use; required", true, true,
	  "", nullptr },
	{ "submeasure", 's', 's', "submeasure to use; optional; depends on the distance measure",
	  false, true, "", nullptr },
	{ "measureopt", 'o', 's', "options for the distance measure; depends on the measure",
	  false, true, "", nullptr },
	{ "distmatfname", 'd', 's', "distance matrix file name; required", true,
	  true, "", nullptr },
	{ "ncores", 'n', 'i', "number of CPU cores to use; optional; default: all",
	  false, true, "", nullptr },
	{ "checkpointdir", 'c', 's', "checkpoint directory to use; default value exists",
	  false, true, "", nullptr },
	{ "printresult", 'p', 's', "print the resulting distance matrix.  Default: false",
	  false, true, "false", nullptr },
    };
    
    std::string checkpointfname = "options.checkpoint";

    unsigned int findoption(std::string name) const;

    public:
        Options(int argc, char **argv);

	void set(const char *key, const char *value) {
	    set(std::string(key), std::string(value));
	};
	void set(const std::string key, const std::string value);

	std::string get(const char *key) const {return get(std::string(key));};
	std::string get(const std::string key) const { return option_defs[findoption(key)].value; };
	unsigned int get_ncores() const { return std::stoi(get("ncores")); }; //### need to get value from option_defs
	bool get_restart() const { return option_defs[findoption("restart")].value.compare("true") == 0; };

        void checkpoint(void);
	void cleancheckpointdir(void);
	void restore(void);
	void print(void);
};

#endif // OPTIONS_H
