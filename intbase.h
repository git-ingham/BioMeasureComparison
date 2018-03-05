#ifndef INTBASE_H
#define INTBASE_H

/*
 * a class to hold the knowledge about mapping bases to ints and vice versa
 */

#include <err.h>
#include <assert.h>
#include <iostream>

const unsigned int alphabet_size = 4; // Number of bases we work with
const unsigned int base_bitmask = 0x3;
const unsigned int base_nbits = 2; // Number of bits used to store a base

static const std::string bases = "ACGT";
static const std::string lcbases = "acgt"; // must match above
static const char mapping[alphabet_size] = { 'A', 'C', 'G', 'T'}; // must match above

// const unsigned int alphabet_size = 2; // Number of bases we work with
// const unsigned int base_bitmask = 0x1;
// const unsigned int base_nbits = 1; // Number of bits used to store a base
// 
// static const std::string bases = "AC";
// static const std::string lcbases = "ac"; // must match above
// static const char mapping[alphabet_size] = { 'A', 'C'}; // must match above


class intbase {
    unsigned int base = begin();  // Documented to be initialized to first legal value.

public:
    intbase(void) {};
    intbase(const char b) {
        base = base_to_int(b);
    };
    intbase(const unsigned int b) {
        if (b < alphabet_size)
            base = b;
        else
            errx(1, "intbase constructor: Invalid int base value b %u should be in [0..%u)", b, alphabet_size);
    };
    intbase(const intbase &i) {
        base = i.base;
    };
    unsigned int get_int() const {
        return base;
    };
    char get_base() const {
        return bases[base];
    };
    void set_base(unsigned int b) {
        if (b < alphabet_size) {
            base = b;
        } else {
            std::cerr << "base " << b << " >= alphabet size " << alphabet_size << std::endl;
            assert(b < alphabet_size);
        }
    };
    void set_base(const char b) {
        set_base(base_to_int(b));
    };
    static unsigned int
    base_to_int(const char base) {
        for (unsigned int i=0; i<alphabet_size; ++i)
            if (base == bases[i] || base == lcbases[i])
                return i;
        std::cerr << "base_to_int: unknown base '" << base << "'" << std::endl;
        abort();
        /*NOTREACHED*/
    };

    static char
    int_to_base(unsigned int value) {
        if (value < alphabet_size)
            return mapping[value];
        // else fatal error
        std::cerr << "int_to_base: invalid base value: " << value << " (max " << alphabet_size << ")" << std::endl;
        assert(value < alphabet_size);

        /*NOTREACHED*/
        return 0;
    };

    intbase& operator=(const intbase &i) {
        if (this == &i)      // Same object?
            return *this;
        base = i.base;
        return *this;
    };
    intbase& operator=(const unsigned int i) {
        assert(i < alphabet_size);
        base = i;
        return *this;
    };
    intbase& operator++(void) { // Allowed to equal intbase::end()
        if (base < alphabet_size)
            ++base;
        else { // base > alphabet_size-1; we should never be here
            std::cerr << "intbase ++ tried to wrap around!" << std::endl;
            assert(base < alphabet_size);
        }
        return *this;
    };
    intbase& operator--(void) {
        if (base > 0)
            --base;
        else // base == alphabet_size-1
            base = alphabet_size-1;
        return *this;
    };
    bool operator<(const intbase& rhs) const {
        return base < rhs.get_int();
    };
    bool operator<(const unsigned int rhs) const {
        return base < rhs;
    };
    bool operator>(const intbase& rhs) const {
        return base > rhs.get_int();
    };
    bool operator==(const intbase& rhs) const {
        return base == rhs.get_int();
    };
    bool operator!=(const intbase& rhs) const {
        return !(*this == rhs);
    };

    static unsigned int begin(void) {
        return 0;
    };
    static unsigned int end(void) {
        return alphabet_size;
    };

    void print(std::string comment = "") {
        std::cout << comment;
        std::cout << this;
    };
    friend std::ostream& operator<< (std::ostream &stream, intbase ib) {
        stream << "{" << std::dec << ib.base << " (" << bases[ib.base] << ")}";
        return stream;
    };

    void test(bool verbose = true) {
        assert(bases.length() == alphabet_size);

        // base_to_int and int_to_base work
        for (unsigned int i=0; i<alphabet_size; ++i) {
            std::cout << "i: " << i << "; base: " << bases[i] << std::endl;
            assert(int_to_base(i) == bases[i]);
            assert(base_to_int(bases[i]) == i);

            char base = intbase::int_to_base(i);
            unsigned int ui = intbase::base_to_int(base);
            assert(ui == i);
            if (verbose) std::cout << "i: " << std::dec << i << " converts to '" << base << "'." << std::endl;

            base = bases.at(i);
            ui = intbase::base_to_int(base);
            assert(ui == i);

            if (verbose) std::cout << "base '" << base << "' converts to " << std::dec << ui << "." << std::endl;
        }
        if (verbose) std::cout << "base_to_int and int_to_base work OK." << std::endl;

        intbase ib; // We do not test ourselves, because we do not know the state of "base"
        if (verbose) ib.print("Newly-created intbase: ");

        // start at min value
        assert(ib.get_int() == begin());

        // ++ operator works
        for (unsigned int i=1; i<alphabet_size; ++i) {
            ++ib;
            if (verbose) {
                std::cout << "i: " << i << "; ib: ";
                ib.print();
            }
            assert(ib.get_base() == int_to_base(i));
        }
        assert(ib.get_int() == end());

        // -- operator works
        for (unsigned int i=alphabet_size; i>0; --i) {
            if (verbose) {
                std::cout << "i: " << i << "; ib: ";
                ib.print();
            }
            assert(ib.get_base() == int_to_base(i-1));
            --ib;
        }
        if (verbose) ib.print("After -- loop, ib is: ");
        assert(ib.get_int() == end()); // last --ib should wrap around

        // >, <, ==, and != operators work
        intbase ib2((unsigned int)1);
        assert(ib2 < ib);
        assert(ib > ib2);
        assert(ib == ib);
        assert(ib2 != ib);
        if (verbose) std::cout << "Relational operators work." << std::endl;
    };
};

namespace std
{
template <>
struct hash<intbase>
{
    size_t operator()(const intbase& ib) const
    {
        return hash<unsigned int>()(ib.get_int());
    }
};
}


#endif // INTBASE_H
