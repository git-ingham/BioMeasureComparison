//! @file kmer.h
//! @brief a kmer is a vector of length k of bases
//! base type is chosen by which include file is used to define "base".

#ifndef KMER_H
#define KMER_H

#include "intbase.h"
// #include "charbase.h"

#include <vector>

// A sequence is either a k-mer or where we get the k-mers
typedef std::vector<base_t> sequence_t;

class kmer {
protected:
    //! @brief the length of the smallest usable kmer.  Need to be able to have prefixes and
    //! suffixes, hence 2

    unsigned int k; //!< @brief the length of the kmer

    void validate_k_min(const unsigned int k_p) {
        std::string init("kmer validate_k: (");
        init += std::to_string(k_p);
        init += std::string(") ");
        // Error checking
        if (k_p < min_k) {
            std::cerr << init + std::string("< min k (") + std::to_string(min_k) + std::string(")") << std::endl;
            assert(k_p >= min_k);
        }

        // Passed error check.
    };

public:
    static const unsigned int min_k = 2;
    static const unsigned int max_k = 14; // 14 appears to be max practical

    kmer(const unsigned int k_p) {
        validate_k_min(k_p);
        k = k_p;
    };
    kmer() {
        std::cerr << "Fatal error: kmer constructor called with no k." << std::endl;
        abort();
    };
    virtual ~kmer() {};

    virtual void set_kmer(const sequence_t seq) {
        std::cerr << "kmer must be subclassed!" << std::endl;
        abort();
    };
    unsigned int get_k(void) const {
        return k;
    };
    virtual sequence_t get_kmer(void) const = 0;
    virtual sequence_t get_prefix(void) const = 0;
    virtual sequence_t get_suffix(void) const = 0;
};

std::ostream& operator<< (std::ostream &stream, sequence_t s) {
    for (unsigned int i=0; i<s.size(); ++i) {
        if (i != 0) stream << " ";
        stream << s[i];
    }
    return stream;
}

#endif // KMER_H
