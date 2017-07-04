#ifndef KMERMETRIC_H
#define KMERMETRIC_H

#include "metric.h"
#include "fasta.h"
#include <map>
#include <string>
#include <math.h>

                 // kmer     count
typedef std::map<std::string, unsigned int> kmer_t;
               // sequence    kmers
typedef std::map<std::string, kmer_t*> kmers_t;

enum variants {euclidean, cosine};

class kmermetric : public metric {
    const unsigned int default_n = 11;
    unsigned int n = default_n;
    const long double halfpi = 2.0 * atanl(1.0);
    kmers_t kmers;
    //variants algorithm = euclidean;
    variants algorithm = cosine;

    void calculate_set(const std::string& seq);

    long double euclideancompare(const std::string& aseq, const std::string& bseq) const;
    long double cosinecompare(const std::string& aseq, const std::string& bseq) const;

    public:
	void init(void) { n = default_n; };
	void init(const unsigned int in) { n = in; };
	void init(const unsigned int in, const fastavec_t& seqs);
	//void init(void) { n = default_n; kmers.erase(kmers.begin()); };
	//void init(const unsigned int in) { n = in; kmers.erase(kmers.begin()); };

	void printdetails() {
	    std::cerr << "kmer distance:" << std::endl;
	    std::cerr << "    k = " << n << std::endl;
	    std::cerr << "    variant: " << atostring(algorithm) << std::endl;
	};

	long double compare(const FastaRecord& a, const FastaRecord& b) const;

	// member functions beyond metric
	unsigned int get_n(void) const { return n; };
	unsigned int set_n(const unsigned int in) { unsigned int old=n; n = in; return old; };

	variants get_algorithm(void) { return algorithm; }
	variants set_algorithm(variants a) { variants old = algorithm; algorithm = a; return old; }
	variants set_algorithm(const std::string& a) { variants old = algorithm; algorithm = stringtoa(a); return old; }

	std::string atostring(variants v);
	variants stringtoa(const std::string& v);

	kmermetric(void) {init();};
	kmermetric(const unsigned int in) {init(in);};

	~kmermetric() {};
};

#endif // KMERMETRIC_H
