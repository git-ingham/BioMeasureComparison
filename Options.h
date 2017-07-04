#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>
#include <iostream>
#include <getopt.h>
#include <thread>
#include <err.h>
#include <stdlib.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

typedef std::map<std::string, std::string> optmap;

class Options {
    friend class boost::serialization::access;
    unsigned int nthreads = 1;
    bool restart = false;
    std::string null = "";
    std::string default_checkpointdir = "./metrictest.checkpoint";
    std::string checkpointfname = "options.checkpoint";
    optmap strvalues = {
        { "progname", "" },
        { "fastafile", "" },
        { "metricname", "" },
        { "metricopts", "" },
        { "distmatfname", "" },
        { "submetricname", "" },
        { "checkpointdname", default_checkpointdir },
        { "nthreads", std::to_string(nthreads) },
        { "restart", "false" },
    };

    void print_usage(void) {
	std::cout << "Usage: " << strvalues["progname"]
		  << " --fasta=fname"
		  << " --metric=metricname"
		  << " --submetric=submetricname"
		  << " --metricopt=options"
		  << " --nthreads=n"
	          << std::endl;
    };
//    void initialize_map(std::string pname) {
//	    strvalues.insert(std::pair<std::string, std::string>("progname", pname));
//            strvalues.insert(std::pair<std::string, std::string>("fastafile", null));
//            strvalues.insert(std::pair<std::string, std::string>("metricname", null));
//            strvalues.insert(std::pair<std::string, std::string>("metricopts", null));
//            strvalues.insert(std::pair<std::string, std::string>("distmatfname", null));
//            strvalues.insert(std::pair<std::string, std::string>("submetricname", null)); // might never be set
//            strvalues.insert(std::pair<std::string, std::string>("checkpointdname", default_checkpointdir));
//            strvalues.insert(std::pair<std::string, std::string>("nthreads", std::to_string(nthreads)));
//            strvalues.insert(std::pair<std::string, std::string>("restart", std::to_string(restart)));
//	};
    void processopts(int argc, char **argv);

    public:
        //Options(std::string pname) { set("progname", pname); };
        Options(int argc, char **argv) {
	    std::string pname(argv[0]);
	    //initialize_map(pname);
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
