#ifndef KMERINT_H
#define KMERINT_H

#include <string>
#include <tuple>
#include <err.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

// This meets standards
typedef uint64_t kmer_storage_t;
const unsigned int max_k = 32;
// Non-standard; cout does not work with it
//typedef unsigned __int128 kmer_storage_t;
//const unsigned int max_k = 64;

const unsigned int min_k = 2; // Need to be able to have prefixes and suffixes

const unsigned int alphabet_size = 4; // Number of bases we work with
const unsigned int base_bitmask = 0x3;
const unsigned int base_nbits = 2; // Number of bits used to store a base
const std::string bases("ACGT");

class kmerint {
    unsigned int k = 0;
    kmer_storage_t kbitmask;  // bit mask for all bits actually used in kmer storage
    kmer_storage_t kmerhash;
    
    void init_bitmask() {
        assert(k > 0);
        kbitmask = 0;
        for (kmer_storage_t i=0; i<k; ++i) {
            kbitmask <<= base_nbits;
            kbitmask |= base_bitmask;
        }
    };
    void init_k(const unsigned int k_p) {
        auto [ok, errmsg] = validate_k(k_p);
        if (!ok) errx(1, "%s", errmsg.c_str());
        k = k_p;
        init_bitmask();
    };

public:
    kmerint(const unsigned int k_p) {
        init_k(k_p);
    };
    kmerint(const unsigned int k_p, const std::string kmer) {
        init_k(k_p);
        setkmer(kmer);
    };
    kmerint(const unsigned int k_p, const kmer_storage_t hash) {
        init_k(k_p);
        setkmerhash(hash);
    };
    ~kmerint() {};
    
    std::tuple<bool,std::string> validate_k(const unsigned int k_p) {
        static std::string init("deBruijn constructor: (");
        init += std::to_string(k_p) + std::string(") ");
        if (k_p > max_k)
            return std::make_tuple(false, init + std::string("> max k (") + std::to_string(max_k) + std::string(")"));
        if (k_p < min_k)
            return std::make_tuple(false, init + std::string("< min k (") + std::to_string(min_k) + std::string(")"));
        return std::make_tuple(true, std::string(""));
    };

    void setkmer(const std::string kmer) {
        kmerhash = string_to_hash(kmer);
    };
    void setkmerhash(const kmer_storage_t kmer) {
        kmerhash = kmer;
    };
    std::string getkmer() {
        return hash_to_string(kmerhash);
    };
    kmer_storage_t getkmerhash() {
        return kmerhash;
    };
    unsigned int get_k() {
        return k;
    };
    
    static inline unsigned int
    base_to_int(const char base) {
        if (base == 'a' || base == 'A')
            return 0;
        else if (base == 'c' || base == 'C')
            return 1;
        else if (base == 'g' || base == 'G')
            return 2;
        else if (base == 't' || base == 'T')
            return 3;
        else // error
            errx(1, "unknown base (%c) in base_to_int", base);
        /*NOTREACHED*/
    };
    
    static inline char
    int_to_base(unsigned int value) {
        static char mapping[alphabet_size] = { 'A', 'C', 'G', 'T' };
        //static char mapping[alphabet_size] = { 'a', 'c', 'g', 't' };
        if (value < alphabet_size)
            return mapping[value];
        else
            errx(1, "int_to_base: invalid base value: %u (max %u)", value, alphabet_size);
    };

    kmer_storage_t string_to_hash(const std::string& kmer) {
        assert(kmer.length() == k);
        kmer_storage_t hash = 0;
        if (kmer.length() != k)
            errx(1, "kmer '%s' length is not k (%u)", kmer.c_str(), k);
        for (unsigned int i=0; i<k; ++i) {
            hash <<= base_nbits;
            hash += base_to_int(kmer.at(i));
        }
        return hash;
    };
    
    std::string hash_to_string(kmer_storage_t kmer) {
        std::string result = "";
        for (unsigned int i=0; i<k; ++i) {
            result += int_to_base(kmer & base_bitmask);
            kmer >>= base_nbits;
        }
        std::reverse(result.begin(),result.end());
        assert(result.length() == k);
        return std::string(result);
    };
    
    unsigned int count_bits(kmer_storage_t bitstring) {
        unsigned int count = 0;
        while (bitstring > 0) {
            if (bitstring & 1) ++count;
            bitstring >>= 1;
        }
        return count;
    };

    void print(const std::string prefix = "") {
        std::cout << prefix << "k: " << std::dec << k << std::endl;
        std::cout << prefix << "kbitmask: 0x" << std::hex << std::setfill('0') << kbitmask << std::endl;
        std::cout << prefix << "kmer: " << hash_to_string(kmerhash) << std::endl;
        std::cout << prefix << "kmerhash: 0x" << std::hex << std::setfill('0') << std::setw(k*base_nbits/4) << kmerhash << std::endl;
    };
    
    void test(const bool verbose = true) {
        if (verbose) print();
        
        // does kbitmask cover all possible values and nothing more?
        // __builtin_popcount is builtin to gcc
        if (verbose) std::cout << "n bits in kbitmask: " << count_bits(kbitmask)<< std::endl;
        if (verbose) std::cout << "n bits in base_bitmask: " << count_bits(base_bitmask) << std::endl;
        assert(count_bits(kbitmask) == count_bits(base_bitmask)*k);
        if (verbose) std::cout << "kbitmask is OK." << std::endl;
                
        // does base_to_int() properly invert int_to_base and vice versa?
        assert(bases.length() == alphabet_size);
        for (unsigned int i=0; i<alphabet_size; ++i) {
            char base = int_to_base(i);
            unsigned int ui = base_to_int(base);
            assert(ui == i);
            if (verbose) std::cout << "i: " << std::dec << i << " converts to '" << base << "'." << std::endl;
            
            base = bases.at(i);
            ui = base_to_int(base);
            assert(ui == i);

            if (verbose) std::cout << "base '" << base << "' converts to " << std::dec << ui << "." << std::endl;
        }
        
        // does string_to_hash properly invert hash_to_string and vice versa?
        std::string kmer("");
        for (unsigned int i=0; i<k; ++i)
            kmer += int_to_base(i % alphabet_size);
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;
        
        kmer_storage_t h = string_to_hash(kmer);
        // /2 because hex is 4 bits per char but we store 2 bases in 4 bits.
        if (verbose) std::cout << "hash of kmer is: 0x" << std::hex << std::setfill('0') << std::setw(k*base_nbits/4) << h << std::endl;
        std::string vv = hash_to_string(h);
        if (verbose) std::cout << "string from hash is: '" << vv << "'" << std::endl;
        assert(vv.compare(kmer) == 0);
        assert(h == string_to_hash(vv));
        
        // Do all setters and getters work properly?
        kmer = "";
        for (unsigned int i=0; i<k; ++i) {
            kmer += int_to_base(i % alphabet_size);
        }
        setkmer(kmer);
        assert(kmer.compare(hash_to_string(kmerhash)) == 0);
        assert(getkmerhash() == kmerhash);
        assert(getkmer().compare(hash_to_string(kmerhash)) == 0);
        //### This test is weak
        for (unsigned int i=0; i<alphabet_size; ++i) {
            setkmerhash(i);
            assert(kmerhash == i);
            assert(hash_to_string(kmerhash).compare(hash_to_string(i)) == 0);
        }
        
        std::cout << "All tests for k = " << std::dec << k << " succeeded." << std::endl;
        if (verbose) std::cout << std::endl;
    };
};

#endif // KMERINT_H
