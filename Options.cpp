#include "Options.h"
#include <err.h>
#include <unistd.h>
#include "utils.h"

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
        ("checkpoint", po::value<std::string>(), "checkpoint directory; default exists")
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

    // no else because optional
    // First, because mandatory options are not madatory in restart
    if (vm.count("restart"))
        set("restart", "true");

    if (vm.count("checkpoint"))
       set("checkpoint", vm["checkpoint"].as<std::string>());
    // no else because optional
    if (vm.count("distmatfname"))
       set("distmatfname", vm["distmatfname"].as<std::string>());
    else if (!restart) {
	std::cerr << "Missing required option '--distmatfname'" << std::endl;
	error = true;
    }
    if (vm.count("fasta"))
       set("fasta", vm["fasta"].as<std::string>());
    else if (!restart) {
	std::cerr << "Missing required option '--fasta'" << std::endl;
	error = true;
    }
    if (vm.count("metric"))
       set("metric", vm["metric"].as<std::string>());
    else if (!restart) {
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

    if (error) {
         std::cerr << desc << std::endl;
	 exit(1);
    }

    if (restart) 
        restore();
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
    if (key.compare("fastafile") == 0) {
	foundkey = true;
	fastafile = value;
	if (!fileexists(fastafile)) 
	    errx(1, "fastafile '%s' does not exist", fastafile.c_str());
    }
    if (key.compare("metricname") == 0) {
	foundkey = true;
	metricname = value;
	// ### No error checking here?
    }
    if (key.compare("submetricname") == 0) {
	foundkey = true;
	submetricname = value;
	// ### No error checking here?
    }
    if (key.compare("metricopts") == 0) {
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

