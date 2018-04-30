//! @file kmer.h
//! @brief abstract class for a kmer
//! Known subclasses are kmerint and kmerstring

#ifndef KMER_H
#define KMER_H

//! @todo a mess of code since this should be split into two classes that inherit from measure: cosine and euclidean

#include <string>
#include <assert.h>
#include <log4cxx/logger.h>

class kmer {
protected:
    //! @brief the length of the kmer
    unsigned int k;

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
    //! @brief the length of the smallest usable kmer.  Need to be able to have prefixes and 
    //! suffixes, hence 2
    static const unsigned int min_k = 2;
    static const unsigned int max_k = 14; // 32 is limit of kmerint, but 14 appears to be max practical
    
    kmer(const unsigned int k_p) {
        validate_k_min(k_p);
        k = k_p;
    };
    kmer() {
        std::cerr << "Fatal error: kmer constructor called with no k." << std::endl; 
        abort();
    };
    virtual ~kmer() {};

    virtual void set_kmer(const std::string kmer) = 0;
    unsigned int get_k(void) const {
        return k;
    };
    virtual std::string get_kmer(void) const = 0;
    virtual std::string get_prefix(void) const = 0;
    virtual std::string get_suffix(void) const = 0;
};

#endif // KMER_H
