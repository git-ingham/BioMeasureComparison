#include "kmermetric.h"
#include "fasta.h"
#include <cmath>       /* pow, acosl */
#include <stdlib.h>     /* exit */
#include <err.h>        /* errx */

void
kmermetric::calculate_set(const std::string& seq)
{
    if (kmers.count(seq) == 0) // Probably true
        kmers.emplace(seq, new kmer_t);
    for (unsigned int i=0; i<seq.length()-n; ++i) {
	std::string kmer = seq.substr(i, n);
	if (kmers.at(seq)->count(kmer) > 0) 
	    kmers.at(seq)->at(kmer) = kmers.at(seq)->at(kmer) + 1;
	else 
	    kmers.at(seq)->emplace(kmer, 1);
    }
}

long double
kmermetric::compare(const FastaRecord& a, const FastaRecord& b) const
{
    const std::string aseq = a.get_seq();
    const std::string bseq = b.get_seq();
    if (aseq.length() == 0)
        errx(1, "kmermetric::compare: aseq is 0-length");
    if (bseq.length() == 0)
        errx(1, "kmermetric::compare: bseq is 0-length");

    //std::cerr << aseq << std::endl;
    //std::cerr << bseq << std::endl;
    //std::cerr << kmers.size() << std::endl;

    // sanity check
    if (kmers.count(aseq) == 0) {
	std::cerr << "kmermetric::compare aseq not pre-calculated!" << std::endl;
	exit(1);
    }
    if (kmers.count(bseq) == 0) {
	std::cerr << "kmermetric::compare bseq not pre-calculated!" << std::endl;
	exit(1);
    }

//    static bool printed = false;
//
//    if (!printed) {
//	std::cerr << "about to choose; alg is " << atostring(algorithm) << std::endl;
//	printed = true;
//    }
    if (algorithm == cosine)
        return cosinecompare(aseq, bseq);
    if (algorithm == euclidean)
        return euclideancompare(aseq, bseq);
    
    // If we are here, it is a bug
    std::cerr << "No matching algorithm for kmermetric::compare." << std::endl;
    exit(1);
}

/*
 * Euclidean distance based on frequency counts
 */
long double
kmermetric::euclideancompare(const std::string& aseq, const std::string& bseq) const
{
    long double dist = 0.0;

    // a -> b
    //std::cerr << aseq << ", " << bseq << std::endl;
    for (kmer_t::const_iterator g = kmers.at(aseq)->begin(); g!=kmers.at(aseq)->end(); ++g) {
        std::string kmer = g->first;
	if (kmers.at(bseq)->count(kmer) > 0) {
	    int t = kmers.at(aseq)->at(kmer) - kmers.at(bseq)->at(kmer);
	    dist += t*t;
	    //std::cerr << kmer << " " << kmers.at(aseq)->at(kmer) << ", "
	    //<< kmers.at(bseq)->at(kmer) << ": " << dist << std::endl;
	}
	else {
	    unsigned int t = kmers.at(aseq)->at(kmer);
	    dist += t*t;
	    //std::cerr << "t: " << t << " dist: " << dist << std::endl;
	}
    }

    //std::cerr << "b -> a" << std::endl;

    // b -> a, but only for the kmers that are in b and not in a
    for (kmer_t::const_iterator g = kmers.at(bseq)->begin(); g!=kmers.at(bseq)->end(); ++g) {
	std::string kmer = g->first;
	if (kmers.at(aseq)->count(kmer) == 0) {
	    unsigned int t = kmers.at(bseq)->at(kmer);
	    dist += t*t;
	}
    }

    // mapped into [0,1]
    //return dist == 0 ? 0 : 1.0 - 1.0/sqrt(dist);
    return sqrt(dist);
}

/*
 * cosine distance based on frequency counts
 * https://en.wikipedia.org/wiki/Cosine_similarity
 */
long double
kmermetric::cosinecompare(const std::string& aseq, const std::string& bseq) const
{
    long double dotproduct = 0.0;
    long double asum = 0;
    long double bsum = 0;

    // all we care about are those kmers in common; others have a 0 product and
    // contribute nothing to the final result.
    for (kmer_t::const_iterator g = kmers.at(aseq)->begin(); g!=kmers.at(aseq)->end(); ++g) {
        std::string kmer = g->first;
	if (kmers.at(bseq)->count(kmer) > 0)
	    dotproduct += kmers.at(aseq)->at(kmer) * kmers.at(bseq)->at(kmer);
	asum += kmers.at(aseq)->at(kmer)*kmers.at(aseq)->at(kmer);
    }

    for (kmer_t::const_iterator g = kmers.at(bseq)->begin(); g!=kmers.at(bseq)->end(); ++g) {
	std::string kmer = g->first;
	bsum += kmers.at(bseq)->at(kmer) * kmers.at(bseq)->at(kmer);
    }

    long double cosine  = dotproduct / (sqrt(asum) * sqrt(bsum));
    if (cosine < -1.0) {
	//std::cerr << "Warning: cosine " << cosine << " is < -1.0." << std::endl;
        cosine = -1.0;
    }
    if (cosine > 1.0) {
	//std::cerr << "Warning: cosine " << cosine << " is > 1.0." << std::endl;
        cosine = 1.0;
    }
    long double result = acosl(cosine)/halfpi;
    if (result <= 2.09629e-10)
        return 0.0;
    else
	return result;
}

variants
kmermetric::stringtoa(const std::string& v)
{
    if (v.compare("euclidean") == 0) return euclidean;
    if (v.compare("cosine") == 0) return cosine;
    errx(1, "Unknown variant in kmermetric::atostring '%s'", v.c_str());
}

std::string 
kmermetric::atostring(variants v)
{
    if (v == euclidean) return "Euclidean";
    if (v == cosine) return "cosine";
    errx(1, "Unknown variant in kmermetric::atostring");
}

void
kmermetric::init(const unsigned int in, const fastavec_t& seqs)
{
    n = in;
    fastavec_t::const_iterator s;
    for (s=seqs.begin(); s != seqs.end(); ++s) {
        calculate_set(s->get_seq());
    }
}
