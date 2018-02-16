#ifndef KMERINT_H
#define KMERINT_H

#include <string>
#include <tuple>
#include <err.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

#include "intbase.h"

// This meets standards
typedef uint64_t kmer_storage_t;
const unsigned int max_k = 32;
// Non-standard; cout does not work with it
//typedef unsigned __int128 kmer_storage_t;
//const unsigned int max_k = 64;

const unsigned int min_k = 2; // Need to be able to have prefixes and suffixes

class kmerint {
    //### Should use intbase and not unsigned int
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
        set_kmer(kmer);
    };
    kmerint(const unsigned int k_p, const kmer_storage_t hash) {
        init_k(k_p);
        set_kmerhash(hash);
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

    void set_kmer(const std::string kmer) {
        kmerhash = string_to_hash(kmer);
    };
    void set_kmerhash(const kmer_storage_t kmer) {
        kmerhash = kmer;
    };
    std::string get_kmer() const {
        return hash_to_string(kmerhash);
    };
    kmer_storage_t get_kmerhash() const {
        return kmerhash;
    };
    unsigned int get_k() const {
        return k;
    };
    kmer_storage_t get_kmerbitmask(void) const {
        return kbitmask;
    };
    std::string get_prefix(void) const {
        return hash_to_string(kmerhash).substr(0,k-1);
    };
    std::string get_suffix(void) const {
        return hash_to_string(kmerhash).substr(1,k-1);
    };
    kmer_storage_t get_prefixhash(void) const {
        return kmerhash >> base_nbits;
    };
    kmer_storage_t get_suffixhash(void) const {
        return kmerhash & (kbitmask >> base_nbits);
    };
    kmer_storage_t maxhash(void) const {
        return pow(alphabet_size, k);
    };

    kmerint operator=(const kmerint &rhs) {
        if (this == &rhs)      // Same object?
            return *this;      // Yes, so skip assignment, and just return *this.
        k = rhs.get_k();
        kbitmask = rhs.get_kmerbitmask();
        kmerhash = rhs.get_kmerhash();
        return *this;
    };

    // Caution: ++ and += operate very differently!
    kmerint& operator++() {
        ++kmerhash;
        kmerhash &= kbitmask;
        return *this;
    };
    kmerint& operator--() {
        --kmerhash;
        kmerhash &= kbitmask;
        return *this;
    };

    kmerint& operator+=(const char base) {
        kmerhash = ((kmerhash << base_nbits) & kbitmask) | intbase::base_to_int(base);
        return *this;
    };
    kmerint operator+(const char base) {
        kmerint result = *this;
        result += base;
        return result;
    };

    kmer_storage_t string_to_hash(const std::string& kmer) const {
        assert(kmer.length() == k);
        kmer_storage_t hash = 0;
        if (kmer.length() != k)
            errx(1, "kmer '%s' length is not k (%u)", kmer.c_str(), k);
        for (unsigned int i=0; i<k; ++i) {
            hash <<= base_nbits;
            hash += intbase::base_to_int(kmer.at(i));
        }
        return hash;
    };

    std::string hash_to_string(kmer_storage_t kmer) const {
        std::string result = "";
        for (unsigned int i=0; i<k; ++i) {
            result += intbase::int_to_base(kmer & base_bitmask);
            kmer >>= base_nbits;
        }
        std::reverse(result.begin(),result.end());
        assert(result.length() == k);
        return std::string(result);
    };

    static unsigned int count_bits(kmer_storage_t bitstring) {
        unsigned int count = 0;
        while (bitstring > 0) {
            if (bitstring & 1) ++count;
            bitstring >>= 1;
        }
        return count;
    };

    void print(const std::string prefix = "") const {
        std::cout << prefix << "kmerhash: 0x" << std::hex << std::setfill('0') << std::setw(k*base_nbits/4) << kmerhash << std::endl;
        std::cout << prefix << "kmer: " << hash_to_string(kmerhash) << std::endl;
        std::cout << prefix << "k: " << std::dec << k << std::endl;
        std::cout << prefix << "kbitmask: 0x" << std::hex << std::setfill('0') << kbitmask << std::endl;
    };

    void test(const bool verbose = true) {
        if (verbose) print();

        // does kbitmask cover all possible values and nothing more?
        if (verbose) std::cout << "n bits in kbitmask: " << count_bits(kbitmask)<< std::endl;
        if (verbose) std::cout << "n bits in base_bitmask: " << count_bits(base_bitmask) << std::endl;
        assert(count_bits(kbitmask) == count_bits(base_bitmask)*k);
        if (verbose) std::cout << "kbitmask is OK." << std::endl;

        // does string_to_hash properly invert hash_to_string and vice versa?
        std::string kmer("");
        for (unsigned int i=0; i<k; ++i)
            kmer += intbase::int_to_base(i % alphabet_size);
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
            kmer += intbase::int_to_base(i % alphabet_size);
        }
        set_kmer(kmer);
        assert(kmer.compare(hash_to_string(kmerhash)) == 0);
        assert(get_kmerhash() == kmerhash);
        assert(get_kmer().compare(hash_to_string(kmerhash)) == 0);
        // ### This test is weak; needs to ensure that all edge cases are covered
        for (unsigned int i=0; i<alphabet_size; ++i) {
            set_kmerhash(i);
            assert(kmerhash == i);
            assert(hash_to_string(kmerhash).compare(hash_to_string(i)) == 0);
        }

        // ### Verify ++ and -- work properly, including wraparound 0 and max value
        kmerint ki(k, 0);
        if (verbose) {
            std::cout << "initial creation with hash 0: " << std::endl;
            ki.print("    ");
        }
        assert(ki.get_kmerhash() == 0);
        // wrap around 0
        --ki;
        if (verbose) {
            std::cout << "after decrement: " << std::endl;
            ki.print("    ");
        }
        assert(ki.get_kmerhash() == kbitmask);
        ++ki;
        if (verbose) {
            std::cout << "after increment: " << std::endl;
            ki.print("    ");
        }
        assert(ki.get_kmerhash() == 0);

        // verify += works
        ki += 'C';
        if (verbose) {
            std::cout << "After += C: " <<  std::endl;
            ki.print("    ");
        }
        assert(ki.get_kmerhash() == intbase::base_to_int('C'));

        std::cout << "All tests for k = " << std::dec << k << " succeeded." << std::endl;
        if (verbose) std::cout << std::endl;
    };
};

#endif // KMERINT_H
