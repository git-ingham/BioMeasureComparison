#ifndef KMERSTRING_H
#define KMERSTRING_H

#include "metric.h"
#include "fasta.h"
#include <map>
#include <string>
#include <math.h>
#include <assert.h>

// sequence     sum of kmer count squares for cosine compare
typedef std::map<std::string, long double> kmersum_t;


class kmerstring {
    //               kmer         count
    typedef std::map<std::string, unsigned int> kmerstring_t;
    //               sequence     kmers
    typedef std::map<std::string, kmerstring_t*> kmerstrings_t;
    
    const unsigned int min_k = 2; // Need to be able to have prefixes and suffixes
    unsigned int k;
    std::string kmer;

public:
    kmerstring(unsigned int k_p) {
        assert(k_p >= min_k);
        k = k_p;
    };

    ~kmerstring() {};

    unsigned int get_k(void) const {
        return k;
    };
    std::string get_kmer(void) const {
        return kmer;
    };
        std::string get_kmer(void) const {
        return kmer;
    };


    friend std::ostream& operator<< (std::ostream &stream, kmerint ki) {
        stream << "{kmer: " << kmer << "; k: " << k "}";
        return stream;
    };    
};

#endif // KMERSTRING_H
