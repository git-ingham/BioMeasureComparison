#include <iostream>
#include <string>
#include "createmetric.h"
#include "editmetric.h"
#include "kmermetric.h"

metric *
createmetric(const std::string& name, const std::string& subname, const std::string& opts, const fastavec_t& seqs)
{
    if (name.compare("edit") == 0) {
	return new editmetric;
    }
    if (name.compare("kmer") == 0) {
	kmermetric *k = new kmermetric();
	if (subname.length() > 0) k->set_algorithm(subname);
	k->init(std::stoi(opts), seqs);
	return k;
    }

    // If still here, then the metric is unknown
    std::cerr << "Unknown metric '" << name << "'" << std::endl;
    std::cerr << "known metrics are: " << "edit, kmer" << std::endl;
    exit(1);
}
