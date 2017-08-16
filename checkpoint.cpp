#include "Options.h"
#include "checkpoint.h"
#include <signal.h>
#include <iostream>
#include <fstream>
#include "utils.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

const std::string fnamebase = "worker";

void
workercheckpoint(unsigned int i, unsigned int workernum, std::string checkpointdir)
{
    // assumes that dir exists; options checkpoint occurs before here
    // Checking will require additional locking and slow the system.
    
    // Not sure if this is the correct way to do this with C++ threads;
    // should work with posix threads.
    sigset_t set, oldset;
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGHUP);
    if (sigprocmask(SIG_BLOCK, &set, &oldset) < 0)
        err(1, "sigprocmask to block signals failed");

    // Locking is not needed because each worker uses a separate file
    //checkpoint_mutex.lock();

    std::string fname = checkpointdir + "/" + fnamebase + std::to_string(workernum) + ".checkpoint";
    std::ofstream cpf;
    cpf.open(fname);
    cpf << "workernum" << std::endl << workernum << std::endl;
    cpf << "i" << std::endl << i << std::endl;
    cpf.close();

    //checkpoint_mutex.unlock();
    if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0)
        err(1, "sigprocmask to unblock signals failed");
}

unsigned int
workerrestore(unsigned int workernum, std::string checkpointdir)
{
    std::ifstream cpf;
    unsigned int lastfinished;

    std::string fname = checkpointdir + "/" + fnamebase + 
                        std::to_string(workernum) + ".checkpoint";
    cpf.open(fname); //### should error check
    lastfinished = std::stoi(restoreoption("workernum", cpf));
    cpf.close();

    return lastfinished;
}

void
Options::cleancheckpointdir(void)
{
    if (checkdir(checkpointdir)) {
	for (fs::directory_entry& x : fs::directory_iterator(checkpointdir)) {
	    std::cout << x.path() << '\n';
	    remove(x.path());
	}
    } else {
        errx(1, "Checkpoint dir '%s' does not exist and I will not create it.",
	    checkpointdir.c_str());
    }
}

void
Options::checkpoint(void)
{
    checkmakedir(checkpointdir); // exits on failure

    // If we are here, the directory exists.
    std::string fname = checkpointdir + "/" + checkpointfname;
    std::ofstream cpf;
    cpf.open(fname);
    if (cpf.fail())
        err(1, "opening '%s' for writing failed", fname.c_str());

    cpf << "ncores" << std::endl << ncores << std::endl;
    cpf << "distmatfname" << std::endl << distmatfname  << std::endl;
    cpf << "fastafile" << std::endl << fastafile  << std::endl;
    cpf << "metricname" << std::endl << metricname  << std::endl;
    cpf << "submetricname" << std::endl << submetricname  << std::endl;
    cpf << "metricopts" << std::endl << metricopts  << std::endl;

    cpf.close();
}

void
Options::restore(void)
{
    // Verify checkpoint dir exists
    if (!checkdir(checkpointdir))
        errx(1, "Checkpoint dir '%s' does not exist.", checkpointdir.c_str());

    // Open options file
    std::string fname = checkpointdir + "/" + checkpointfname;
    std::ifstream cpf;
    cpf.open(fname);
    if (cpf.fail())
        err(1, "opening '%s' for reading failed", fname.c_str());

    // Parse file to restore options.  Annoyingly silly and should be handled by
    // some kind of structure that improved generality.  Or, even better, a
    // library function for save and restore.
    set("ncores", restoreoption("ncores", cpf));
    set("distmatfname", restoreoption("distmatfname", cpf));
    set("fastafile", restoreoption("fastafile", cpf));
    set("metricname", restoreoption("metricname", cpf));
    set("submetricname", restoreoption("submetricname", cpf));
    set("metricopts", restoreoption("metricopts", cpf));

    // verify that this is the end of file
    if (!cpf.eof()) {
        std::cerr << "Warning, extra, ignored info in '" << fname << "'" << std::endl;
	std::string line;
        if (std::getline(cpf, line))
	    std::cerr << "Warning, first ignored line is'" << line << "'" << std::endl;
	else
	    std::cerr << "Warning, bug in code; no data to get" << std::endl;
    }

    cpf.close();

    std::cout << "Restoring options from checkpoint file" << std::endl << fname << std::endl;
    std::cout << "ncores" << std::endl << ncores << std::endl;
    std::cout << "distmatfname " << std::endl << distmatfname << std::endl;
    std::cout << "fastafile " << std::endl << fastafile << std::endl;
    std::cout << "metricname " << std::endl << metricname << std::endl;
    std::cout << "submetricname " << std::endl << submetricname << std::endl;
    std::cout << "metricopts " << std::endl << metricopts << std::endl;
}

// Check label and return value.
std::string
restoreoption(const std::string label, std::ifstream& cpf)
{
    std::string value, inlabel;

    if (!std::getline(cpf, inlabel)) 
        errx(1, "Missing '%s' label", label.c_str());
    chomp(inlabel);
    if (inlabel.compare(label) != 0) 
        errx(1, "Expecting '%s' label but found '%s'", label.c_str(), inlabel.c_str());
    if (!std::getline(cpf, value)) 
        errx(1, "Missing '%s' value", label.c_str());
    chomp(value);
    return value;
}

