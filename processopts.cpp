#include <string>

#include "processopts.h"

void
print_usage()
{
    extern std::string progname;

    std::cout << "Usage: " << progname
	<< " --fasta=fname --metric=metricname --submetric=submetricname --metricopt=options"
	<< std::endl;
}

void 
processopts(int argc, char **argv)
{
    extern std::string fastafile, metricname, submetricname, metricopts;

    // Option processing
    while (1) {
	int c;
        static struct option long_options[] = {
	    {"fasta", required_argument, 0, 'f'},
	    {"metric", required_argument, 0, 'm'},
	    {"submetric", optional_argument, 0, 's'},
	    {"metricopt", optional_argument, 0, 'o'},
	};
	int option_index = 0;

	c = getopt_long (argc, argv, "f:", long_options, &option_index);
	if (c == -1) break;

	switch (c) {
	case 'f':
	    fastafile = optarg;
	    break;
	case 'm':
	    metricname = optarg;
	    break;
	case 's':
	    submetricname = optarg;
	    break;
	case 'o':
	    metricopts = optarg;
	    break;

	case '?':
	    /* getopt_long already printed an error message. */
	    break;

	default:
	    abort ();
	}
    }

    if (fastafile.empty()) {
        std::cerr << "Missing required flag '-f' or '--fasta'" << std::endl;
	print_usage();
	exit(1);
    }
    if (metricname.empty()) {
        std::cerr << "Missing required flag '-m' or '--metric'" << std::endl;
	print_usage();
	exit(1);
    }
}
