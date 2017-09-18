#include "Options.h"

#include <err.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include "utils.h"
#include "createmetric.h"


Options::Options(int argc, char **argv)
{
    auto validateboolean = [](const std::string value) {
	if (value.compare("true") == 0 || value.compare("false") == 0)
	    return std::string("");
	return std::string("Boolean value '" + value +
	                   "' is neither 'true' nor 'false'.");
    };
    auto validatefile = [](const std::string value) {
        if (fileexists(value))
	    return std::string("");
	return std::string("File '" + value + "' does not exist.");
    };
    auto validatedir = [](const std::string value) {
        if (direxists(value))
	    return std::string("");
	return std::string("Directory '" + value + "' does not exist.");
    };
    auto validatecores = [](const std::string value) {
        unsigned int ncores = stoi(value);
	if (ncores > 0 && ncores <= std::thread::hardware_concurrency())
	    return std::string("");

	return std::string("ncores '" + value + "' is greater than system cores (" +
			   std::to_string(std::thread::hardware_concurrency()) + ").");
    };
    auto novalidation = [](const std::string value) {return std::string("");};

    bool error = false;

    // cannot do this initialization in Options.h class defn
    // even if we are not using lambdas but declared functions
    option_defs[findoption("restart")].checksanity = validateboolean;
    option_defs[findoption("fasta")].checksanity = validatefile;
    option_defs[findoption("measure")].checksanity = validatemetric;
    option_defs[findoption("submeasure")].checksanity = novalidation;
    option_defs[findoption("measureopt")].checksanity = novalidation;
    option_defs[findoption("distmatfname")].checksanity = novalidation;
    option_defs[findoption("ncores")].checksanity = validatecores;
    option_defs[findoption("checkpointdir")].checksanity = novalidation;

    // Default values
    set("checkpointdir", "./metrictest.checkpoint");
    set("restart", "false");
    set("ncores", std::to_string(std::thread::hardware_concurrency()));

    option long_opts[nopts];
    std::string short_opts("");
    for (unsigned int i=0; i<nopts; ++i) {
        long_opts[i].name = strdup(option_defs[i].name.c_str());
	long_opts[i].has_arg = option_defs[i].has_argument ? 1 : 0;
	long_opts[i].flag = NULL;
	long_opts[i].val = option_defs[i].shortflag;

        short_opts.append(std::string(1, option_defs[i].shortflag));
	if (option_defs[i].has_argument)
	    short_opts.append(":");
    }

    while (true) {
        int opt = getopt_long(argc, argv, short_opts.c_str(), long_opts, nullptr);

	if (opt == -1) break;
	
	bool found = false;
	for (unsigned int i=0; i<nopts; ++i) {
	    if (opt == option_defs[i].shortflag) {
	        found = true;

		if (option_defs[i].has_argument) {
		    //### might want to hold off overwriting this?
		    option_defs[i].value = std::string(optarg);
		} else { //### is this the only valid way of dealing with no-arg opts?
		    option_defs[i].value = "true";
		}

		std::string err = (option_defs[i].checksanity)(option_defs[i].value);
		if (err.length() > 0) {
		    std::cerr << "Error in " << option_defs[i].name << ": ";
		    std::cerr << err << std::endl;
		    error = true;
		}
	    } else if (opt == '?') {
		// Need tp print the usage statement
		found = true;
	        error = true;
	    }
	}
	if (!found) {
	    std::cerr << "Unknown option: '" << opt << "'" << std::endl;
	    error = true;
	}
    }

    if (get_restart()) 
        restore();

    /* verify all mandatory options are set. */
    for (unsigned int i=0; i<nopts; ++i) {
        if (option_defs[i].mandatory && !get_restart() && option_defs[i].value.length() == 0) {
	    std::cerr << "Missing mandatory option '" << option_defs[i].name
	              << "' (" << option_defs[i].description << ")"
		      << std::endl;
	    error = true;
	}
    }

    if (error) {
        std::cerr << "One or more errors detected." << std::endl;
	std::cerr << "Valid options:" << std::endl;
	for (unsigned int i=0; i<nopts; ++i) {
	    std::cerr << "    --";
	    std::cerr << option_defs[i].name << "\t" <<
	                 option_defs[i].description << std::endl;
	}
	exit(1);
    }
}

unsigned int
Options::findoption(std::string name) const
{
    for (unsigned int i=0; i<nopts; ++i) {
	if (option_defs[i].name.compare(name) == 0)
	    return i;
    }
    
    std::cerr << "Options::findoption: No key '" << name << "'" << std::endl;
    abort();
}

void
Options::set(const std::string key, const std::string value)
{
    if (value.find('\n') != std::string::npos)
        errx(1, "key '%s' value '%s' contains an embedded newline", key.c_str(), value.c_str());

    std::string (*validate)(std::string) = option_defs[findoption(key)].checksanity;
    std::string err = validate(value);

    if (err.length() == 0) 
	option_defs[findoption(key)].value = value;
    else {
        std::cerr << err << std::endl;
	abort();
    }
}

void
Options::print(void)
{
    for (unsigned int i=0; i<nopts; ++i) {
        std::cout << option_defs[i].name << ": " << option_defs[i].value << std::endl;
    }
}
