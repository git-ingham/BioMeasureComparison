#ifndef KMER_H
#define KMER_H

// ### a mess of code since this should be split into two classes that inherit from measure: cosine and euclidean

#include "measure.h"
#include "kmerint.h"
#include "kmerstring.h"

enum variants {euclidean, cosine};

class kmer : public measure {
//     variants algorithm = euclidean;
    variants algorithm = cosine;
    const long double halfpi = 2.0 * atanl(1.0);

    void calculate_set(const std::string& seq);
    long double calculate_sum(const std::string& seq);

    long double euclideancompare(const std::string& aseq, const std::string& bseq) const;
    long double cosinecompare   (const std::string& aseq, const std::string& bseq) const;
    kmersum_t sums; // for caching cosine calculated data
    
    // ### n is probablu obsolete
    const unsigned int default_n = 11;
    unsigned int n = default_n;
    kmerstrings_t kmers;
    

public:
    void printdetails() {
        std::cerr << "kmer distance:" << std::endl;
        std::cerr << "    k = " << n << std::endl;
        std::cerr << "    variant: " << atostring(algorithm) << std::endl;
    };

    long double compare(const FastaRecord& a, const FastaRecord& b) const;

    /*
    * Euclidean distance based on frequency counts
    */
    long double
    euclideancompare(const std::string& aseq, const std::string& bseq) const
    {
        long double dist = 0.0;

        // a -> b
        //std::cerr << aseq << ", " << bseq << std::endl;
        for (kmerstring_t::const_iterator g = kmers.at(aseq)->begin(); g!=kmers.at(aseq)->end(); ++g) {
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
        for (kmerstring_t::const_iterator g = kmers.at(bseq)->begin(); g!=kmers.at(bseq)->end(); ++g) {
            std::string kmer = g->first;
            if (kmers.at(aseq)->count(kmer) == 0) {
                unsigned int t = kmers.at(bseq)->at(kmer);
                dist += t*t;
            }
        }

        // mapped into [0,1]
        //return dist == 0 ? 0 : 1.0 - 1.0/sqrt(dist);

        // Actual Euclidean distance
        //return sqrt(dist);

        // Save time and do not do the sqrt, since the relative distances
        // remain the same, even if the absolute distances are different.
        return dist;
    };

    long double
    calculate_sum(const std::string& seq)
    {
        kmerstring_t::const_iterator end;
        unsigned int count;
        long double sum = 0.0;

        end = kmers.at(seq)->end();
        for (kmerstring_t::const_iterator g = kmers.at(seq)->begin(); g!=end; ++g) {
            std::string kmer = g->first;
            count = kmers.at(seq)->at(kmer);
            sum += count*count;
        }
        return sqrt(sum);
    };

    /*
     * cosine distance based on frequency counts
     * https://en.wikipedia.org/wiki/Cosine_similarity
     */
    long double
    cosinecompare(const std::string& aseq, const std::string& bseq) const
    {
        long double dotproduct = 0.0;
        kmerstring_t::const_iterator end;

        // for the dot product, all we care about are those kmers in common;
        // others have a 0 product and contribute nothing to the final result.
        end = kmers.at(aseq)->end();
        for (kmerstring_t::const_iterator g = kmers.at(aseq)->begin(); g!=end; ++g) {
            std::string kmer = g->first;
            if (kmers.at(bseq)->count(kmer) > 0) {
                dotproduct += kmers.at(aseq)->at(kmer) * kmers.at(bseq)->at(kmer);
            }
        }

        long double cosine  = dotproduct / (sums.at(aseq) * sums.at(bseq));
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
    };

    void
    kmerstring::calculate_set(const std::string& seq)
    {
        if (kmers.count(seq) == 0) { // Probably true
            kmers.emplace(seq, new kmerstring_t);
        }
        for (unsigned int i=0; i<seq.length()-n; ++i) {
            std::string kmer = seq.substr(i, n);
            if (kmers.at(seq)->count(kmer) > 0)
                kmers.at(seq)->at(kmer) = kmers.at(seq)->at(kmer) + 1;
            else
                kmers.at(seq)->emplace(kmer, 1);
        }
        if (algorithm == cosine)
            sums.emplace(seq, calculate_sum(seq));
    };

    long double
    kmerstring::compare(const FastaRecord& a, const FastaRecord& b) const
    {
        const std::string aseq = a.get_seq();
        const std::string bseq = b.get_seq();
        if (aseq.length() == 0)
            errx(1, "kmerstring::compare: aseq is 0-length");
        if (bseq.length() == 0)
            errx(1, "kmerstring::compare: bseq is 0-length");

        // sanity check
        if (kmers.count(aseq) == 0) {
            std::cerr << "kmerstring::compare aseq not pre-calculated!" << std::endl;
            exit(1);
        }
        if (kmers.count(bseq) == 0) {
            std::cerr << "kmerstring::compare bseq not pre-calculated!" << std::endl;
            exit(1);
        }

        if (algorithm == cosine)
            return cosinecompare(aseq, bseq);
        if (algorithm == euclidean)
            return euclideancompare(aseq, bseq);

        // If we are here, it is a bug
        std::cerr << "No matching algorithm for kmerstring::compare." << std::endl;
        exit(1);
    };

    variants
    kmerstring::stringtoa(const std::string& v)
    {
        if (v.compare("euclidean") == 0) return euclidean;
        if (v.compare("cosine") == 0) return cosine;
        errx(1, "Unknown variant in kmerstring::atostring '%s'", v.c_str());
    };

    std::string
    kmerstring::atostring(variants v)
    {
        if (v == euclidean) return "Euclidean";
        if (v == cosine) return "cosine";
        errx(1, "Unknown variant in kmerstring::atostring");
    };

    void
    kmer(const unsigned int k, const fastavec_t& seqs)
    {
        n = in;
        fastavec_t::const_iterator s;
        for (s=seqs.begin(); s != seqs.end(); ++s) {
            calculate_set(s->get_seq());
        }
    };

    variants get_algorithm(void) {
        return algorithm;
    }
    variants set_algorithm(variants a) {
        variants old = algorithm;
        algorithm = a;
        return old;
    }
    variants set_algorithm(const std::string& a) {
        variants old = algorithm;
        algorithm = stringtoa(a);
        return old;
    }

    std::string atostring(variants v);
    variants stringtoa(const std::string& v);
};

#endif // KMER_H
