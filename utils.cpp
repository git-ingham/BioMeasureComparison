#include "utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>

//### Should throw an exception for different failures
bool
direxists(std::string dir)
{
    struct stat info;
    const char *dname = dir.c_str();

    if (stat(dname, &info) < 0) {
        if (errno == ENOENT) {
            return false;
        } else
            err(1, "stat on '%s' failed", dname);
    }
    if (S_ISDIR(info.st_mode))
	return true;
    else 
	err(1, "directory '%s' is not a directory.", dname);
}

// Remove trailing line ending characters
void
chomp(std::string& line)
{
    line.erase(line.find_last_not_of("\r\n")+1);
}

// ### should thrown an exception instead of exiting on failure
bool
checkmakedir(std::string dir)
{
    struct stat info;
    const char *dname = dir.c_str();

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

bool
fileexists(std::string fname)
{
    struct stat info;
    if (stat(fname.c_str(), &info) < 0) {
	if (errno == ENOENT)
	    return false;
	else 
	    err(1, "Options::fileexists stat failed");
    }
    
    // verify it is a real file.
    if (S_ISREG(info.st_mode)) 
        return true;
    
    errx(1, "'%s' is not a real file", fname.c_str());
}

