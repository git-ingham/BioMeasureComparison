#include "Options.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

void
Options::set(const std::string key, const std::string value)
{
    if (value.find('\n') != std::string::npos)
        errx(1, "key '%s' value '%s' contains an embedded newline", key.c_str(), value.c_str());

    strvalues.at(key) = value;

    if (key.compare("nthreads") == 0) {
	nthreads = stoi(value);
	if (nthreads > std::thread::hardware_concurrency())
	    errx(1, "nthreads '%u' is greater than system cores (%u)",
		 nthreads, std::thread::hardware_concurrency());
    }
    if (key.compare("restart") == 0) {
	if (value.compare("true") == 0) {
	    restart = true;
	} else {
	    if (value.compare("false") == 0)
		restart = false;
	    else 
		errx(1, "Value '%s' for restart is neither 'true' nor 'false'", value.c_str());
	}
    }
}

void 
Options::processopts(int argc, char **argv)
{
    extern std::string fastafile, metricname, submetricname, metricopts, distmatfname;
    extern std::string checkpointfname;

    strvalues.at("progname") = argv[0];

    // Option processing
    while (1) {
	int c;
        static struct option long_options[] = {
	    {"fasta", required_argument, 0, 'f'},
	    {"metric", required_argument, 0, 'm'},
	    {"submetric", optional_argument, 0, 's'},
	    {"metricopt", optional_argument, 0, 'o'},
	    {"distmatfname", required_argument, 0, 'd'},
	    {"nthreads", required_argument, 0, 't'},
	    {"checkpoint", required_argument, 0, 'c'},
	    {"restart", no_argument, 0, 'r'},
	};
	int option_index = 0;

	c = getopt_long (argc, argv, "f:", long_options, &option_index);
	if (c == -1) break;

	switch (c) {
	case 'r':
	    set("restart", "true");
	    break;
	case 't':
	    set("nthreads", optarg);
	    break;
	case 'c':
	    set("checkpointfname", optarg);
	    break;
	case 'd':
	    strvalues["distmatfname"] = optarg;
	    break;
	case 'f':
            strvalues["fastafile"] = optarg;
	    break;
	case 'm':
	    strvalues["metricname"] = optarg;
	    break;
	case 's':
	    strvalues["submetricname"] = optarg;
	    break;
	case 'o':
	    strvalues["metricopts"] = optarg;
	    break;

	case '?':
	    /* getopt_long already printed an error message. */
	    break;

	default:
	    abort ();
	}
    }

    if (strvalues.at("fastafile").empty()) {
        std::cerr << "Missing required flag '-f' or '--fasta'" << std::endl;
	print_usage();
	exit(1);
    }
    if (strvalues.at("metricname").empty()) {
        std::cerr << "Missing required flag '-m' or '--metric'" << std::endl;
	print_usage();
	exit(1);
    }
    if (nthreads <= 0) {
        std::cerr << "nthreads (" << nthreads << ") <= 0 must be > 0" << std::endl;
	print_usage();
	exit(1);
    }
}

void
Options::checkpoint(void)
{
    struct stat info;

    const char *dname = get("checkpointdname").c_str();

    std::cerr << "dname: " << dname << std::endl;

    // If dir does not exist, create it if possible
    if (stat(dname, &info) < 0) {
        if (errno == ENOENT) {
	    warnx("checkpoint directory '%s' does not exist; about to create it.", dname);
	    if (mkdir(dname, 0755) < 0) {
	        err(1, "directory '%s' does not exist and creating it failed", dname);
	    }
	} else 
	    err(1, "stat on '%s' failed", dname);
    }

    // If we are here, the directory exists.
    std::string fname = get("checkpointdname") + "/" + checkpointfname;
    std::ofstream cpf;
    cpf.open(fname);

    for (optmap::const_iterator it = strvalues.begin(); it != strvalues.end(); ++it) {
	cpf << it->first << std::endl;
	cpf << it->second << std::endl;
    }
    cpf.close();
}
