#ifndef INTBASE_H
#define INTBASE_H

/*! @file intbase.h
 *  @brief A biological sequence base stored as an integer and utilities to work with it
 */

#include <err.h>
#include <iostream>
#include <log4cxx/logger.h>

const unsigned int alphabet_size = 4;  //!< Number of bases we work with
const unsigned int base_bitmask = 0x3; //!< bitmask for a single base in an integer
const unsigned int base_nbits = 2;     //!< Number of bits used to store a base

static const std::string bases = "ACGT>";   //!< All legal bases plus end indicator
static const std::string lcbases = "acgt>"; //!< Lower-case version of legal bases.  Must match bases
static const char mapping[alphabet_size+1] = { 'A', 'C', 'G', 'T', '>'}; //!< character to base value mapping.  must match bases

// These lines were used for debugging to simplify the graph
// const unsigned int alphabet_size = 2; // Number of bases we work with
// const unsigned int base_bitmask = 0x1;
// const unsigned int base_nbits = 1; // Number of bits used to store a base
//
// static const std::string bases = "AC";
// static const std::string lcbases = "ac"; // must match above
// static const char mapping[alphabet_size] = { 'A', 'C'}; // must match above

/*! @class intbase
 * @brief a class to hold the knowledge about mapping sequence bases to integers and vice versa
 */

class intbase {
    //! The base value
    unsigned int base;

    // logging
    log4cxx::LoggerPtr logger = nullptr;

    void init_logging(void) {
        logger = log4cxx::Logger::getLogger("intbase");
    };

public:
    intbase(void) {
        init_logging();
        base = begin(); // initialized to first legal value unless via a constructor with an initial value.
    };
    intbase(const char b) {
        init_logging();
        base = base_to_int(b);
    };
    intbase(const unsigned int b) {
        init_logging();
        if (b <= alphabet_size)
            base = b;
        else {
            LOG4CXX_FATAL(logger, "intbase constructor: Invalid int base value b " << b << " should be in [0.." << alphabet_size << "].  " << alphabet_size << " is invalid, but is the end indicator.");
            abort();
        }
    };
    intbase(const intbase &i) {
        init_logging();
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
            LOG4CXX_FATAL(logger, "base " << b << " >= alphabet size " << alphabet_size);
            abort();
        }
    };
    void set_base(const char b) {
        set_base(base_to_int(b));
    };
    static unsigned int
    base_to_int(const char base) {
        for (unsigned int i=0; i<alphabet_size; ++i) {
            if (base == bases[i] || base == lcbases[i])
                return i;
        }
        log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("intbase"));
        LOG4CXX_FATAL(logger, "base_to_int: unknown base '" << base << "'");
        abort();
        /*NOTREACHED*/
    };

    /**
    * @brief Convert from an unsigned integer to the character it represents.  The value 
    * >' is used to indicate the end value that is not a legal base, but is a legal value to allow loops to run.
    * 
    * @param value The integer to convert to the corresponding character base
    * @return char value of the base.
    */
    static char
    int_to_base(unsigned int value) {
        if (value < alphabet_size) {
            return mapping[value];
        } else if (value == alphabet_size) {
            return '>';
        } else { // fatal error
            log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("intbase"));

            LOG4CXX_FATAL(logger, "int_to_base: invalid base value: " << value << " (max " << alphabet_size << ")");
            abort();
        }
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
        if (i >= alphabet_size) {
            LOG4CXX_FATAL(logger, "i >= alphabet_size");
            abort();
        }
        base = i;
        return *this;
    };
    intbase& operator++(void) { // Allowed to equal intbase::end()
        if (base < alphabet_size)
            ++base;
        else if (base == alphabet_size) {
            // do nothing; we are at the end and cannot increment more
        }
        else { // base > alphabet_size; we should never be here
            LOG4CXX_FATAL(logger, "intbase ++ on too-large value!");
            abort();
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
    //! @todo need rest of relational operators

    //! the first legal value
    static unsigned int begin(void) {
        return 0;
    };
    //! one past the last legal value
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

    //! @brief Unit test for intbase class
    void test() {
        if (bases.length() != alphabet_size+1) {
            LOG4CXX_FATAL(logger, "number of bases does not equal alphabet_size+1!");
            abort();
        }

        // base_to_int and int_to_base work
        for (unsigned int i=0; i<alphabet_size; ++i) {
            LOG4CXX_TRACE(logger, "i: " << i << "; base: " << bases[i]);
            if (int_to_base(i) != bases[i]) {
                LOG4CXX_FATAL(logger, "int_to_base(i) != bases[i]");
                abort();
            }
            if (base_to_int(bases[i]) != i) {
                LOG4CXX_FATAL(logger, "base_to_int(bases[i]) != i");
                abort();
            }

            char base = int_to_base(i);
            unsigned int ui = base_to_int(base);
            if (ui != i) {
                LOG4CXX_FATAL(logger, "base_to_int(bases[i]) != i");
                abort();
            }

            LOG4CXX_TRACE(logger, "i: " << std::dec << i << " converts to '" << base << "'.");

            base = bases.at(i);
            ui = intbase::base_to_int(base);
            if (ui != i) {
                LOG4CXX_FATAL(logger, "ui != i");
                abort();
            }

            LOG4CXX_TRACE(logger, "base '" << base << "' converts to " << std::dec << ui << ".");
        }
        LOG4CXX_INFO(logger, "base_to_int and int_to_base work OK.");

        intbase ib; // We do not test ourselves, because we do not know the state of "base"
        LOG4CXX_DEBUG(logger, "Newly-created intbase: " << ib);

        // start at min value
        if (ib.get_int() != begin()) {
            LOG4CXX_FATAL(logger, "ib.get_int() != begin()");
            abort();
        }

        // ++ operator works
        for (unsigned int i=1; i<end(); ++i) {
            ++ib;
            LOG4CXX_DEBUG(logger, "i: " << i << "; ib: " << ib);
            if (ib.get_base() != int_to_base(i)) {
                LOG4CXX_FATAL(logger, "ib.get_base() != int_to_base(i)");
                abort();
            }
        }
        LOG4CXX_DEBUG(logger, "After ++ loop, ib is: " << ib);

        // -- operator works
        for (unsigned int i=alphabet_size; i>0; --i) {
            LOG4CXX_DEBUG(logger, "i: " << i << "; ib: " << ib);
            if (ib.get_base() != int_to_base(i-1)) {
                LOG4CXX_FATAL(logger, "ib.get_base() != int_to_base(i-1)");
                abort();
            }
            --ib;
        }
        LOG4CXX_DEBUG(logger, "After -- loop, ib is: " << ib);

        // >, <, ==, and != operators work
        intbase ib2((unsigned int)1);
        if (!(ib2 < ib)) {
            LOG4CXX_FATAL(logger, "ib2 >= ib");
            abort();
        }
        if (!(ib > ib2)) {
            LOG4CXX_FATAL(logger, "ib <= ib2");
            abort();
        }
        if (!(ib == ib)) {
            LOG4CXX_FATAL(logger, "ib != ib");
            abort();
        }
        if (!(ib2 != ib)) {
            LOG4CXX_FATAL(logger, "ib2 == ib");
            abort();
        }
        LOG4CXX_INFO(logger, "Relational operators work.");
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
