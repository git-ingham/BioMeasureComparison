#include "Options.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

Options::Options(int argc, char **argv)
{
    bool error = false;
    
    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce help message")
        ("fasta", po::value<std::string>(), "fasta file containing sequences; required")
	("metric", po::value<std::string>(), "distance measure to use; required")
        ("submetric", po::value<std::string>(),
	 "submetric to use; optional; depends on the distance measure")
        ("metricopt", po::value<std::string>(),
	 "options for the distance measure; depends on the measure")
        ("distmatfname", po::value<std::string>(), "distance matrix file name; required")
        ("ncores", po::value<std::string>(),
	 "number of CPU cores to use; optional; default: 1")
        ("checkpoint", po::value<std::string>(), "checkpoint directory; required only if restarting")
        ("restart",
	 "restart from checkpoint; optional; default: not restarting from checkpoint")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
	exit(1);
    }

    if (vm.count("checkpoint"))
       set("checkpoint", vm["checkpoint"].as<std::string>());
    // no else because optional
    if (vm.count("distmatfname"))
       set("distmatfname", vm["distmatfname"].as<std::string>());
    else {
	std::cerr << "Missing required option '--distmatfname'" << std::endl;
	error = true;
    }
    if (vm.count("fasta"))
       set("fasta", vm["fasta"].as<std::string>());
    else {
	std::cerr << "Missing required option '--fasta'" << std::endl;
	error = true;
    }
    if (vm.count("metric"))
       set("metric", vm["metric"].as<std::string>());
    else {
	std::cerr << "Missing required option '--metric'" << std::endl;
	error = true;
    }
    if (vm.count("submetric"))
       set("submetric", vm["submetric"].as<std::string>());
    // no else because optional
    if (vm.count("metricopt"))
       set("metricopt", vm["metricopt"].as<std::string>());
    // no else because optional
    if (vm.count("ncores"))
       set("ncores", vm["ncores"].as<std::string>());
    // no else because optional
    if (vm.count("restart"))
        set("restart", "true");

    if (error) {
         std::cerr << desc << std::endl;
	 exit(1);
    }
};

std::string
Options::get(const std::string key) const
{
    if (key.compare("checkpointdir") == 0)
        return checkpointdir;
    if (key.compare("distmatfname") == 0)
        return distmatfname;
    if (key.compare("fasta") == 0)
        return fastafile;
    if (key.compare("metric") == 0)
	return metricname;
    if (key.compare("submetric") == 0)
	return submetricname;
    if (key.compare("metricopt") == 0)
	return metricopts;
    if (key.compare("ncores") == 0)
	return std::to_string(ncores);
    if (key.compare("restart") == 0)
        return restart ? "true" : "false";

    std::cerr << "Options::get: No known key '" << key << "'" << std::endl;
    abort();
}

void
Options::set(const std::string key, const std::string value)
{
    bool foundkey = false;

    if (value.find('\n') != std::string::npos)
        errx(1, "key '%s' value '%s' contains an embedded newline", key.c_str(), value.c_str());

    if (key.compare("checkpoint") == 0) {
	foundkey = true;
	checkpointdir = value;
	// ### No error checking here?
    }
    if (key.compare("distmatfname") == 0) {
	foundkey = true;
	distmatfname = value;
	// ### No error checking here?
    }
    if (key.compare("fasta") == 0) {
	foundkey = true;
	fastafile = value;
	if (!fileexists(fastafile)) 
	    errx(1, "fastafile '%s' does not exist", fastafile.c_str());
    }
    if (key.compare("metric") == 0) {
	foundkey = true;
	metricname = value;
	// ### No error checking here?
    }
    if (key.compare("submetric") == 0) {
	foundkey = true;
	submetricname = value;
	// ### No error checking here?
    }
    if (key.compare("metricopt") == 0) {
	foundkey = true;
	metricopts = value;
	// ### No error checking here?
    }
    if (key.compare("ncores") == 0) {
	foundkey = true;
	ncores = stoi(value);
	if (ncores > std::thread::hardware_concurrency())
	    errx(1, "ncores '%u' is greater than system cores (%u)",
		 ncores, std::thread::hardware_concurrency());
    }
    if (key.compare("restart") == 0) {
	foundkey = true;
	if (value.compare("true") == 0 || value.compare("1") == 0) {
	    restart = true;
	} else {
	    if (value.compare("false") == 0 || value.compare("0") == 0)
		restart = false;
	    else 
		errx(1, "Value '%s' for restart is neither 'true' nor 'false'", value.c_str());
	}
    }

    if (! foundkey) 
        errx(1, "Options::set: No known key '%s'", key.c_str());
}

bool
Options::fileexists(std::string fname)
{
    struct stat info;
    if (stat(fname.c_str(), &info) < 0) {
	if (errno == ENOENT)
	    return false;
	else 
	    err(1, "Options::fileexists stat failed");
    }
    return true;
}

void
Options::cleancheckpointdir(void)
{
    for (directory_entry& x : directory_iterator(p))
	cout << x.path() << '\n';
}

void
Options::checkpoint(void)
{
    checkmakedir(checkpointdir); // exits on failure

    // If we are here, the directory exists.
    std::string fname = checkpointdir + "/" + checkpointfname;
    std::ofstream cpf;
    cpf.open(fname);

    cpf << "ncores" << std::endl << ncores << std::endl;
    cpf << "distmatfname " << std::endl << distmatfname  << std::endl;
    cpf << "fastafile " << std::endl << fastafile  << std::endl;
    cpf << "metricname " << std::endl << metricname  << std::endl;
    cpf << "submetricname " << std::endl << submetricname  << std::endl;
    cpf << "metricopts " << std::endl << metricopts  << std::endl;

    cpf.close();
}

// ### should thrown an exception instead of exiting on failure
bool
Options::checkmakedir(std::string checkpointdir)
{
    struct stat info;
    const char *dname = checkpointdir.c_str();

    //std::cerr << "dname: " << dname << std::endl;

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
    return true;
}

