#include "createmetric.h"

//### This needs to be in metric.{h,cpp} as a constructor?
// or as a data structure with details?

metric *
createmetric(const Options opts, const fastavec_t& seqs)
//createmetric(const std::string& name, const std::string& subname, const std::string& opts, const fastavec_t& seqs)
{
    if (opts.get("measure").compare("edit") == 0) {
	return new editmetric(opts.get("measureopt"));
    }
    if (opts.get("measure").compare("kmer") == 0) {
	kmermetric *k = new kmermetric();
	if (opts.get("submeasure").length() > 0)
	    k->set_algorithm(opts.get("submeasure"));
	k->init(std::stoi(opts.get("measureopt")), seqs);
	return k;
    }

    // If still here, then the metric is unknown
    std::cerr << "Unknown measure '" << opts.get("measurename") << "'" << std::endl;
    std::cerr << "known measures are: " << "edit, kmer" << std::endl;
    exit(1);
}

std::string
validatemetric(std::string name)
{
    if (name.compare("edit") == 0) return "";
    if (name.compare("kmer") == 0) return "";
    return std::string("Unknown measure '") + name + std::string("'.\n") +
           std::string("Known measures are: edit, kmer.");
}
