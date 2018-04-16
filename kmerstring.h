#ifndef KMERSTRING_H
#define KMERSTRING_H

#include "kmer.h"
#include "FastaRecord.h"
#include <map>
#include <string>
#include <math.h>
#include <assert.h>

// sequence     sum of kmer count squares for cosine compare
typedef std::map<std::string, long double> kmersum_t;


class kmerstring : public kmer {
    //               kmer         count
    typedef std::map<std::string, unsigned int> kmerstring_t;
    //               sequence     kmers
    typedef std::map<std::string, kmerstring_t*> kmerstrings_t;

    std::string kmer_s;

public:
    using kmer::kmer;

    ~kmerstring() {};

    void set_kmer(const std::string kmer_p) {
        kmer_s = kmer_p;
    };
    
    std::string get_kmer(void) const {
        return kmer_s;
    };
    std::string get_prefix(void) const {
        return kmer_s.substr(0, kmer_s.length() - 1);
    };
    std::string get_suffix(void) const {
        return kmer_s.substr(1);
    };
    bool operator<(const kmerstring& rhs) const {
        return kmer_s.compare(rhs.kmer_s) < 0;
    };

    friend std::ostream& operator<< (std::ostream &stream, kmerstring ks) {
        stream << "{kmer: " << ks.kmer_s << "; k: " << ks.k << "}";
        return stream;
    };
};

#endif // KMERSTRING_H
