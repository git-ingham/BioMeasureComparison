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
#include <log4cxx/logger.h>

#include "kmer.h"
#include "intbaseDNA.h"
#include "intbase2.h"
#include "intbaseOPs.h"

// The specific intbase subclass we use
//typedef intbaseDNA intbase_t;
// typedef intbase2 intbase_t;
typedef intbaseOPs intbase_t;

std::ostream& operator<<(std::ostream& os, const unsigned __int128 i) noexcept
{
  std::ostream::sentry s(os);
  if (s) {
    unsigned __int128 tmp = i < 0 ? -i : i;
    char buffer[128];
    char *d = std::end(buffer);
    do {
      --d;
      *d = "0123456789"[tmp % 10];
      tmp /= 10;
    } while (tmp != 0);
    if (i < 0) {
      --d;
      *d = '-';
    }
    int len = std::end(buffer) - d;
    if (os.rdbuf()->sputn(d, len) != len) {
      os.setstate(std::ios_base::badbit);
    }
  }
  return os;
}

/*!
 * @class kmerint
 * @brief a kmer stored as a hash in an integer
 */
class kmerint : public kmer {
    // This meets standards vs below which is non-standard and has problems but can handle longer k-mers
//     typedef uint64_t kmer_storage_t;
    // Code for up to 32 should be OK for DNA, but 14 caused memory issues in deBruijn Graph
    //const unsigned int max_k = 32; // defined in kmer.h
    // Non-standard; cout does not work with it
    typedef unsigned __int128 kmer_storage_t;
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
            LOG4CXX_FATAL(logger, init + std::string("> max k (") + std::to_string(max_k) + std::string(")"));
            abort();
        }
    };

    // logging
    log4cxx::LoggerPtr logger = nullptr;
    void init_logging(void) {
        logger = log4cxx::Logger::getLogger("kmerint");
    };


public:
    kmerint(const unsigned int k_p) : kmer(k_p) {
        init_logging();
        validate_k_max(k_p);
        init_consts();
        set_kmerhash(0);
    };
    kmerint(const kmerint &k_p) : kmer(k_p.k) {
        init_logging();
        validate_k_max(k_p.k);  // should always succeed since k_p called this also.
        init_consts();
        kmerhash = k_p.kmerhash;
        kmerbitmask = k_p.kmerbitmask;
    };
    kmerint(const unsigned int k_p, const sequence_t kmer_p) : kmer(k_p) {
        init_logging();
        validate_k_max(k_p);
        init_consts();
        set_kmer(kmer_p);
    };
    kmerint(const unsigned int k_p, const kmer_storage_t hash) : kmer(k_p) {
        init_logging();
        validate_k_max(k_p);
        init_consts();
        set_kmerhash(hash);
    };
    kmerint(void) : kmer(2) { // 2 is bogus, but we want to die here and not there.
        init_logging();
        LOG4CXX_FATAL(logger, "kmerint constructor called with no k; this is illegal.");
        abort();
    };

    ~kmerint() {};

    void set_kmer(const sequence_t kmer_p) {
        kmerhash = vector_to_hash(kmer_p);
    };
    void set_kmerhash(const kmer_storage_t kmer_p) {
        kmerhash = kmer_p;
    };
    sequence_t get_kmer() const {
        return hash_to_vector(kmerhash);
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
    sequence_t get_prefix(void) const {
        sequence_t vec = hash_to_vector(kmerhash);
        sequence_t::const_iterator begin = vec.begin();
        sequence_t::const_iterator end = vec.begin() + (k-1);
        return sequence_t(begin, end);
    };
    kmer_storage_t get_prefixhash(void) const {
        return kmerhash >> base_nbits;
    };
    sequence_t get_suffix(void) const {
        sequence_t vec = hash_to_vector(kmerhash);
        sequence_t::const_iterator begin = vec.begin() + 1;
        sequence_t::const_iterator end = vec.end();
        return sequence_t(begin, end);
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
    // These operators exist because the deBruijnNode code needs to create a complete graph,
    // which requires iterating through all possible values.
    kmerint& operator++() {
        // ++ increment the hash value to the next base value
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
    // k += b means add b at the right end of k, dropping the highest-order (leftmost) base
    kmerint& operator+=(base_t b) {
        intbase_t ib(b);
        kmerhash = ((kmerhash << ib.get_nbits()) & kmerbitmask) | ib.base_to_int(b);
        return *this;
    };
    kmerint& operator+=(const intbase_t &b) {
        kmerhash = ((kmerhash << b.get_nbits()) & kmerbitmask) | b.get_int();
        return *this;
    };

    kmerint operator+(const intbase_t &b) {
        kmerint result = *this;
        result += b;
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

    kmer_storage_t vector_to_hash(const sequence_t kmer) const {
        intbase_t ib;
        kmer_storage_t hash = 0;
        if (kmer.size() != k) {
            std::stringstream kmerstr;
            kmerstr << kmer;
            LOG4CXX_FATAL(logger, "kmer '" << kmerstr.str() << "' length (" << kmer.size() << ") is not k (" << k << ")");
            abort();
        }
        for (unsigned int i=0; i<k; ++i) {
            hash <<= base_nbits;
            hash += ib.base_to_int(kmer[i]);
        }
        return hash;
    };
    sequence_t hash_to_vector(kmer_storage_t kmer) const {
        intbase_t ib;
        sequence_t result;
        for (unsigned int i=0; i<k; ++i) {
            result.push_back(ib.int_to_base(kmer & base_bitmask));
            kmer >>= base_nbits;
        }
        std::reverse(result.begin(),result.end());
        assert(result.size() == k);
        return sequence_t(result);
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
        stream << "kmer: " << ki.hash_to_vector(ki.kmerhash) << "; ";
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

        // does string_to_vector properly invert hash_to_vector and vice versa?
        sequence_t kmer;
        for (unsigned int i=0; i<ki.k; ++i)
            kmer.push_back(ib.int_to_base(i % ki.alphabet_size));
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;

        kmer_storage_t h = ki.vector_to_hash(kmer);
        if (verbose) std::cout << "hash of kmer is: 0x" << std::hex << std::setfill('0') << std::setw(ki.k*ki.base_nbits/4) << h << std::endl;
        sequence_t vv = ki.hash_to_vector(h);
        if (verbose) std::cout << "vector from hash is: '" << vv << "'" << std::endl;
        assert(vv == kmer);
        assert(h == ki.vector_to_hash(vv));

        // Do all setters and getters work properly?
        kmer.clear();
        for (unsigned int i=0; i<ki.k; ++i) {
            kmer.insert(kmer.begin(), ib.int_to_base(i % ki.alphabet_size));
        }
        ki.set_kmer(kmer);
        sequence_t answer = ki.hash_to_vector(ki.kmerhash);
        if (kmer != answer) {
            std::stringstream kmerstr;
            kmerstr << kmer;
            std::stringstream answerstr;
            answerstr << answer;

            LOG4CXX_FATAL(ki.logger, "kmer " << kmerstr.str() << " != answer " << answerstr.str());
            abort();
        }
        assert(ki.get_kmerhash() == ki.kmerhash);
        assert(ki.get_kmer() == answer);

        for (unsigned int i=0; i<ki.alphabet_size; ++i) {
            ki.set_kmerhash(i);
            assert(ki.kmerhash == i);
            assert(ki.hash_to_vector(ki.kmerhash) == ki.hash_to_vector(i));
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

        sequence_t k3k;
        for (unsigned int i=0; i<ki.k; ++i)
            k3k.push_back(ib.int_to_base(1));
        kmerint k3(ki.k, k3k);
        if (verbose) std::cout << "k3: " << k3 << std::endl;
        assert(k3.k == ki.k);
        assert(k3.kmerbitmask == ki.kmerbitmask);
        assert(k3.kmerhash == ki.vector_to_hash(k3k));

        kmerint k4(ki.k, ki.vector_to_hash(k3k));
        if (verbose) std::cout << "k4: " << k4 << std::endl;
        assert(k4.k == ki.k);
        assert(k4.kmerbitmask == ki.kmerbitmask);
        assert(k4.kmerhash == ki.vector_to_hash(k3k));

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
        ib.set_base(ib.begin()+1);
        k5 += ib;
        if (verbose) {
            std::cout << "After += " << ib << ": " <<  std::endl;
            k5.print("    ");
        }
        assert(k5.get_kmerhash() == ib.begin()+1); // k5 hash was 0 before +=, so one base is all that is there

        // Check for regression of an annoying bug
        //! @todo: will not work with OPs
        sequence_t s;
        for (unsigned int i=0; i<ki.k; ++i)
            s.push_back(ib.int_to_base(0));
        kmerint k6(ki.k, s);
        intbase_t ib5(ib.int_to_base(1));
        kmerint k7(ki.k);
        k7 = k6 + ib5;
        sequence_t correct = k5.get_prefix();
        correct.push_back(ib.int_to_base(1));
        if (k7.get_kmer() != correct) {
            std::cout << k6 << " + " << ib5 << " != " << correct << std::endl;
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
