#include "createmetric.h"

metric *
createmetric(const Options opts, const fastavec_t& seqs)
//createmetric(const std::string& name, const std::string& subname, const std::string& opts, const fastavec_t& seqs)
{
    if (opts.get("metric").compare("edit") == 0) {
	return new editmetric;
    }
    if (opts.get("metric").compare("kmer") == 0) {
	kmermetric *k = new kmermetric();
	if (opts.get("submetric").length() > 0)
	    k->set_algorithm(opts.get("submetric"));
	k->init(std::stoi(opts.get("metricopt")), seqs);
	return k;
    }

    // If still here, then the metric is unknown
    std::cerr << "Unknown metric '" << opts.get("metricname") << "'" << std::endl;
    std::cerr << "known metrics are: " << "edit, kmer" << std::endl;
    exit(1);
}
