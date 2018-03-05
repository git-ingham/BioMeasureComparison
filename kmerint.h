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
// Code for up to 32 should be OK, but 14 caused memory issues in deBruijn Graph
const unsigned int max_k = 32;
// Non-standard; cout does not work with it
//typedef unsigned __int128 kmer_storage_t;
//const unsigned int max_k = 64;

const unsigned int min_k = 2; // Need to be able to have prefixes and suffixes

class kmerint {
    unsigned int k;
    kmer_storage_t kmerbitmask;  // bit mask for all bits actually used in kmer storage
    kmer_storage_t kmerhash;

    void init_bitmask() {
        assert(k > 0);
        kmerbitmask = 0;
        for (kmer_storage_t i=0; i<k; ++i) {
            kmerbitmask <<= base_nbits;
            kmerbitmask |= base_bitmask;
        }
    };
    void init_k(const unsigned int k_p) {
        std::string init("kmerint validate_k: (");
        init += std::to_string(k_p) + std::string(") ");
        // Error checking
        if (k_p > max_k) {
            std::cerr << init + std::string("> max k (") + std::to_string(max_k) + std::string(")") << std::endl;
            assert(k_p <= max_k);
        }
        if (k_p < min_k) {
            std::cerr << init + std::string("< min k (") + std::to_string(min_k) + std::string(")") << std::endl;
            assert(k_p >= min_k);
        }

        // Passed error check.
        k = k_p;
        init_bitmask();
    };

public:
    kmerint(const kmerint &k_p) {
        k = k_p.k;
        kmerhash = k_p.kmerhash;
        kmerbitmask = k_p.kmerbitmask;
    };
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
    unsigned int get_k() {
        return k;
    };
    kmer_storage_t get_kmerbitmask(void) const {
        return kmerbitmask;
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
        return kmerhash & (kmerbitmask >> base_nbits);
    };
    kmer_storage_t maxhash(void) const {
        return (alphabet_size << k)-1;
    };
    kmer_storage_t end(void) const {
        return pow(alphabet_size, k);
    };
    kmer_storage_t begin(void) const {
        return 0;
    };

    kmerint operator=(const kmerint &rhs) {
        if (this == &rhs)      // Same object?
            return *this;      // Yes, so skip assignment, and just return *this.
        k = rhs.k;
        kmerbitmask = rhs.get_kmerbitmask();
        kmerhash = rhs.get_kmerhash();
        return *this;
    };

    // Caution: ++ and += operate very differently!
    kmerint& operator++() {
        // ++ increment the hash value
        ++kmerhash;
        if (kmerhash == end())
            return *this; // Special handling for the end case
        kmerhash &= kmerbitmask;
        return *this;
    };
    kmerint& operator--() {
        --kmerhash;
        kmerhash &= kmerbitmask;
        return *this;
    };
    kmerint& operator+=(const char base) {
        kmerhash = ((kmerhash << base_nbits) & kmerbitmask) | intbase::base_to_int(base);
        return *this;
    };
    // Caution: ++ and += operate very differently!
    kmerint& operator+=(const intbase &base) {
        // Add the base onto the existing kmer, leftmost base goes away
        kmerhash = ((kmerhash << base_nbits) & kmerbitmask) | base.get_int();
        return *this;
    };
//     kmerint operator+(const char base) {
//         kmerint result = *this;
//         result += base;
//         return result;
//     };
    kmerint operator+(const intbase &base) {
        kmerint result = *this;
        result += base;
        return result;
    };
    bool operator<(const kmer_storage_t rhs) {
        return kmerhash < rhs;
    };
    bool operator>(const kmer_storage_t rhs) {
        return kmerhash > rhs;
    };
    bool operator==(const kmer_storage_t rhs) {
        return kmerhash == rhs;
    };
    bool operator==(const kmerint rhs) const {
        return kmerhash == rhs.kmerhash;
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
        std::cout << prefix << *this << std::endl;
    };
    friend std::ostream& operator<< (std::ostream &stream, kmerint ki) {
        stream << "{kmerhash: 0x" << std::hex << std::setfill('0') << std::setw(ki.k*base_nbits/4);
        stream << ki.kmerhash << "; ";
        stream << "kmer: " << ki.hash_to_string(ki.kmerhash) << "; ";
        stream << "k: " << ki.k << "; ";
        stream << "kmerbitmask: 0x" << std::hex << std::setfill('0') << ki.kmerbitmask << "}";
        return stream;
    };

    void test(const bool verbose = true) {
        if (verbose) print();

        // does kbitmask cover all possible values and nothing more?
        if (verbose) std::cout << "n bits in kmerbitmask: " << count_bits(kmerbitmask)<< std::endl;
        if (verbose) std::cout << "n bits in base_bitmask: " << count_bits(base_bitmask) << std::endl;
        assert(count_bits(kmerbitmask) == count_bits(base_bitmask)*k);
        if (verbose) std::cout << "kmerbitmask is OK." << std::endl;

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

        for (unsigned int i=0; i<alphabet_size; ++i) {
            set_kmerhash(i);
            assert(kmerhash == i);
            assert(hash_to_string(kmerhash).compare(hash_to_string(i)) == 0);
        }

        // Verify ++ and -- work properly, including wraparound 0 and max value
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
        assert(ki.get_kmerhash() == kmerbitmask);
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

namespace std
{
template <>
struct hash<kmerint>
{
    size_t operator()(const kmerint& ki) const
    {
        return hash<unsigned int>()(ki.get_kmerhash());
    }
};
}

#endif // KMERINT_H
