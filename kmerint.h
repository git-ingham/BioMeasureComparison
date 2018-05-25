//! @file kmerint.h
//! @brief implenentation of a kmer stored as a hash in an integer

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

#include "kmer.h"
#include "intbaseDNA.h"
#include "intbase2.h"
//#include "intbaseOPs.h"

// The specific intbase subclass we use
typedef intbaseDNA intbase_t;
// typedef intbase2 intbase_t;
// typedef intbaseOPs intbase_t;

/*!
 * @class kmerint
 * @brief a kmer stored as a hash in an integer
 */
class kmerint : public kmer {
    // This meets standards vs below which is non-standard and has problems but can handle longer k-mers
    typedef uint64_t kmer_storage_t;
    // Code for up to 32 should be OK for DNA, but 14 caused memory issues in deBruijn Graph
    //const unsigned int max_k = 32; // defined in kmer.h
    // Non-standard; cout does not work with it
//     typedef unsigned __int128 kmer_storage_t;
//     const unsigned int max_k = 64;

    kmer_storage_t kmerbitmask;  // bit mask for all bits actually used in kmer storage
    kmer_storage_t kmerhash;

    // These are cached here from the intbase subclass we use
    unsigned int base_nbits;
    unsigned int base_bitmask;
    unsigned int alphabet_size;

    void init_consts() {
        intbase_t ib;
        base_bitmask = ib.get_bitmask();
        base_nbits = ib.get_nbits();
        alphabet_size = ib.get_alphabetsize();

        kmerbitmask = 0;
        for (kmer_storage_t i=0; i<k; ++i) {
            kmerbitmask <<= base_nbits;
            kmerbitmask |= base_bitmask;
        }
    };

    void validate_k_max(const unsigned int k_p) {
        // superclass validates against min, nut not max k
        std::string init("kmerint validate_k: (");
        init += std::to_string(k_p);
        init += std::string(") ");
        // Error checking
        if (k_p > max_k) {
            std::cerr << init + std::string("> max k (") + std::to_string(max_k) + std::string(")") << std::endl;
            assert(k_p <= max_k);
        }
    };

public:
    kmerint(const unsigned int k_p) : kmer(k_p) {
        validate_k_max(k_p);
        init_consts();
        set_kmerhash(0);
    };
    kmerint(const kmerint &k_p) : kmer(k_p.k) {
        validate_k_max(k_p.k);  // should always succeed since k_p called this also.
        init_consts();
        kmerhash = k_p.kmerhash;
        kmerbitmask = k_p.kmerbitmask;
    };
    kmerint(const unsigned int k_p, const std::string kmer_p) : kmer(k_p) {
        validate_k_max(k_p);
        init_consts();
        set_kmer(kmer_p);
    };
    kmerint(const unsigned int k_p, const kmer_storage_t hash) : kmer(k_p) {
        validate_k_max(k_p);
        init_consts();
        set_kmerhash(hash);
    };
    kmerint(void) : kmer(2) { // 2 is bogus, but we want to die here and not there.
        std::cerr << "kmerint constructor called with no k; this is illegal." << std::endl;
        abort();
    };

    ~kmerint() {};

    void set_kmer(const std::string kmer_p) {
        kmerhash = string_to_hash(kmer_p);
    };
    void set_kmerhash(const kmer_storage_t kmer_p) {
        kmerhash = kmer_p;
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
    kmer_storage_t get_prefixhash(void) const {
        return kmerhash >> base_nbits;
    };
    std::string get_suffix(void) const {
        return hash_to_string(kmerhash).substr(1,k-1);
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
    // These operators exist because the deBruijnNode code needs to crete a complete graph,
    // which requires iterating through all possible values.
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

    // Caution: ++ and += operate very differently!
    kmerint& operator+=(const char base) {
        intbase_t ib(base);
        kmerhash = ((kmerhash << ib.get_nbits()) & kmerbitmask) | ib.base_to_int(base);
        return *this;
    };
    kmerint& operator+=(const intbase &base) {
        // Add the base onto the existing kmer, leftmost base goes away
        kmerhash = ((kmerhash << base.get_nbits()) & kmerbitmask) | base.get_int();
        return *this;
    };
    kmerint operator+(const intbase &base) {
        kmerint result = *this;
        result += base;
        return result;
    };
    bool operator<(const kmer_storage_t rhs) const {
        return kmerhash < rhs;
    };
    bool operator<(const kmerint& rhs) const {
        return kmerhash < rhs.kmerhash;
    };
    bool operator<(const kmerint* rhs) const {
        return kmerhash < rhs->kmerhash;
    };
    bool operator>(const kmer_storage_t rhs) const {
        return rhs < kmerhash;
    };
    bool operator>(const kmerint rhs) const {
        return kmerhash > rhs.kmerhash;
    };
    bool operator==(const kmer_storage_t rhs) const {
        return kmerhash == rhs;
    };
    bool operator==(const kmerint rhs) const {
        return kmerhash == rhs.kmerhash;
    };

    kmer_storage_t string_to_hash(const std::string& kmer) const {
        intbase_t ib;
        kmer_storage_t hash = 0;
        if (kmer.length() != k)
            errx(1, "kmer '%s' length is not k (%u)", kmer.c_str(), k);
        for (unsigned int i=0; i<k; ++i) {
            hash <<= base_nbits;
            hash += ib.base_to_int(kmer.at(i));
        }
        return hash;
    };

    std::string hash_to_string(kmer_storage_t kmer) const {
        intbase_t ib;
        std::string result = "";
        for (unsigned int i=0; i<k; ++i) {
            result += ib.int_to_base(kmer & base_bitmask);
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
        std::cout << prefix << this << std::endl;
    };
    friend std::ostream& operator<< (std::ostream &stream, kmerint& ki) {
        stream << "{kmerhash: 0x" << std::hex << std::setfill('0') << std::setw(ki.k*ki.base_nbits/4);
        stream << ki.kmerhash << "; ";
        stream << "kmer: " << ki.hash_to_string(ki.kmerhash) << "; ";
        stream << "k: " << ki.k << "; ";
        stream << "kmerbitmask: 0x" << std::hex << std::setfill('0') << ki.kmerbitmask << "}";
        return stream;
    };

    friend void test_kmerint(kmerint& ki, const bool verbose = true) {
        intbase_t ib;

        if (verbose) ki.print();

        // does kbitmask cover all possible values and nothing more?
        if (verbose) std::cout << "n bits in kmerbitmask: " << count_bits(ki.kmerbitmask)<< std::endl;
        if (verbose) std::cout << "n bits in base_bitmask: " << count_bits(ki.base_bitmask) << std::endl;
        assert(count_bits(ki.kmerbitmask) == count_bits(ki.base_bitmask)*ki.k);
        assert(ki.get_kmerbitmask() == ki.kmerbitmask);
        if (verbose) std::cout << "kmerbitmask is OK." << std::endl;

        // does string_to_hash properly invert hash_to_string and vice versa?
        std::string kmer("");
        for (unsigned int i=0; i<ki.k; ++i)
            kmer += ib.int_to_base(i % ki.alphabet_size);
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;

        kmer_storage_t h = ki.string_to_hash(kmer);
        // /2 because hex is 4 bits per char but we store 2 bases in 4 bits.
        if (verbose) std::cout << "hash of kmer is: 0x" << std::hex << std::setfill('0') << std::setw(ki.k*ki.base_nbits/4) << h << std::endl;
        std::string vv = ki.hash_to_string(h);
        if (verbose) std::cout << "string from hash is: '" << vv << "'" << std::endl;
        assert(vv.compare(kmer) == 0);
        assert(h == ki.string_to_hash(vv));

        // Do all setters and getters work properly?
        kmer = "";
        for (unsigned int i=0; i<ki.k; ++i) {
            kmer += ib.int_to_base(i % ki.alphabet_size);
        }
        ki.set_kmer(kmer);
        assert(kmer.compare(ki.hash_to_string(ki.kmerhash)) == 0);
        assert(ki.get_kmerhash() == ki.kmerhash);
        assert(ki.get_kmer().compare(ki.hash_to_string(ki.kmerhash)) == 0);

        for (unsigned int i=0; i<ki.alphabet_size; ++i) {
            ki.set_kmerhash(i);
            assert(ki.kmerhash == i);
            assert(ki.hash_to_string(ki.kmerhash).compare(ki.hash_to_string(i)) == 0);
        }

        // tests for each of the constructors
        kmerint k1(ki.k);
        if (verbose) std::cout << "k1: " << k1 << std::endl;
        assert(k1.k == ki.k);
        assert(k1.kmerbitmask == ki.kmerbitmask);
        assert(k1.kmerhash == 0);

        kmerint k2(ki);
        if (verbose) std::cout << "k2: " << k2 << std::endl;
        assert(k2.k == ki.k);
        assert(k2.kmerbitmask == ki.kmerbitmask);
        assert(k2.kmerhash == ki.kmerhash);

        std::string k3k(ki.k,'C');
        kmerint k3(ki.k, k3k);
        if (verbose) std::cout << "k3: " << k3 << std::endl;
        assert(k3.k == ki.k);
        assert(k3.kmerbitmask == ki.kmerbitmask);
        assert(k3.kmerhash == ki.string_to_hash(k3k));

        kmerint k4(ki.k, ki.string_to_hash(k3k));
        if (verbose) std::cout << "k4: " << k4 << std::endl;
        assert(k4.k == ki.k);
        assert(k4.kmerbitmask == ki.kmerbitmask);
        assert(k4.kmerhash == ki.string_to_hash(k3k));

        // Verify ++ and -- work properly, including wraparound 0 and max value
        kmerint k5(ki.k, 0);
        if (verbose) std::cout << "k5 initial creation with hash 0: " << k5 << std::endl;
        assert(k5.get_kmerhash() == 0);
        ++k5;
        if (verbose) {
            std::cout << "after increment: " << k5 << std::endl;
        }
        assert(k5.get_kmerhash() == 1);
        --k5;
        if (verbose) {
            std::cout << "after decrement: " << k5 << std::endl;
        }
        assert(k5.kmerhash == 0);

        //! @todo test wrapping around with ++ and --

        // verify += works
        k5 += 'C';
        if (verbose) {
            std::cout << "After += C: " <<  std::endl;
            k5.print("    ");
        }
        assert(k5.get_kmerhash() == ib.base_to_int('C')); //! @todo this is a bogus check that happens to work.  Should not be comparing a two-element kmer with a single base.

        // Check for regression of an annoying bug
        std::string s(ki.k, 'A');
        kmerint k6(ki.k, s);
        intbase_t ib5('C');
        kmerint k7(ki.k);
        k7 = k6 + ib5;
        std::string correct = k5.get_prefix() + 'C';
        if (k7.get_kmer().compare(correct) != 0) {
            std::cout << k6 << " + C != " << correct << std::endl;
            abort();
        }

        std::cout << "All tests for k = " << std::dec << ki.k << " succeeded." << std::endl;
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
