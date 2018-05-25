#ifndef INTBASE_H
#define INTBASE_H

/*! @file intbase.h
 *  @brief A sequence base stored as an integer and utilities to work with it.
 * This class *must* be subclassed.
 */

#include <err.h>
#include <iostream>
#include <log4cxx/logger.h>

/*! @class intbase
 * @brief a class to hold the knowledge about mapping sequence bases to integers and vice versa
 */

class intbase {
    // !!! bases *must* be initiaized in a subclass.
protected:
    std::string bases;   //!< All legal bases plus end indicator (>)
    unsigned int alphabet_size = 0; //!< Number of bases we work with
    unsigned int base_bitmask;  //!< bitmask for a single base in an integer
    unsigned int base_nbits;    //!< Number of bits used to store a base
    std::vector<char> mapping; //!< character to base value mapping.  Must match bases

    //! The base value
    unsigned int base;

    // logging
    log4cxx::LoggerPtr logger = nullptr;
    void init_logging(void) {
        logger = log4cxx::Logger::getLogger("intbase");
    };

    // Everything depends on the length of bases
    void set_consts() {
        alphabet_size = bases.length()-1;
        base_nbits = 0;
        base_bitmask = 0;
        unsigned int t = alphabet_size;
        while (t >>= 1) {
            base_bitmask |= 1 << base_nbits;
            ++base_nbits;
        }
        for (unsigned int i=0; i< bases.length(); ++i) {
            mapping.push_back(bases[i]);
        }
    };

public:
    intbase() {
        init_logging();
        base = begin(); // initialized to first legal value unless via a constructor with an initial value.
    };

    unsigned int get_alphabetsize() {
        return alphabet_size;
    }
    unsigned int get_int() const {
        return base;
    };
    char get_base() const {
        return bases[base];
    };
    void set_base(unsigned int b) {
        if (b < get_alphabetsize()) {
            base = b;
        } else {
            LOG4CXX_FATAL(logger, "base " << b << " >= alphabet size " << get_alphabetsize());
            abort();
        }
    };
    void set_base(const char b) {
        set_base(base_to_int(b));
    };
    unsigned int base_to_int(const char base) {
        for (unsigned int i=0; i<get_alphabetsize(); ++i) {
            if (base == bases[i] || tolower(base) == bases[i])
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
    char int_to_base(unsigned int value) {
        if (value < get_alphabetsize()) {
            return mapping[value];
        } else if (value == get_alphabetsize()) {
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
        if (base < get_alphabetsize())
            ++base;
        else if (base == get_alphabetsize()) {
            // do nothing; we are at the end and cannot increment more
        }
        else { // base > get_alphabetsize(); we should never be here
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
    unsigned int end(void) {
        return get_alphabetsize();
    };

    void print(std::string comment = "") {
        std::cout << comment;
        std::cout << this;
    };

    //! @brief Unit test for intbase class
    //! subclasses need their own tests specific to the subclass
    //! ibp must be a freshly-created instance
    friend void test_intbase(intbase& ibp) {
        if (ibp.alphabet_size == 0) {
            LOG4CXX_FATAL(ibp.logger, "test_intbase must be called on a subclass!");
            abort();
        }

        if (ibp.bases.length() != ibp.get_alphabetsize()+1) {
            LOG4CXX_FATAL(ibp.logger, "number of bases does not equal alphabet_size+1!");
            abort();
        }

        // base_to_int and int_to_base work
        for (unsigned int i=0; i<ibp.get_alphabetsize(); ++i) {
            LOG4CXX_TRACE(ibp.logger, "i: " << i << "; base: " << ibp.bases[i]);
            if (ibp.int_to_base(i) != ibp.bases[i]) {
                LOG4CXX_FATAL(ibp.logger, "int_to_base(i) != bases[i]");
                abort();
            }
            if (ibp.base_to_int(ibp.bases[i]) != i) {
                LOG4CXX_FATAL(ibp.logger, "base_to_int(bases[i]) != i");
                abort();
            }

            char base = ibp.int_to_base(i);
            unsigned int ui = ibp.base_to_int(base);
            if (ui != i) {
                LOG4CXX_FATAL(ibp.logger, "base_to_int(bases[i]) != i");
                abort();
            }

            LOG4CXX_TRACE(ibp.logger, "i: " << std::dec << i << " converts to '" << base << "'.");

            base = ibp.bases.at(i);
            ui = ibp.base_to_int(base);
            if (ui != i) {
                LOG4CXX_FATAL(ibp.logger, "ui != i");
                abort();
            }

            LOG4CXX_TRACE(ibp.logger, "base '" << base << "' converts to " << std::dec << ui << ".");
        }
        LOG4CXX_INFO(ibp.logger, "base_to_int and int_to_base work OK.");

        LOG4CXX_DEBUG(ibp.logger, "Newly-created intbase: " << ibp);

        // start at min value
        if (ibp.get_int() != ibp.begin()) {
            LOG4CXX_FATAL(ibp.logger, "ib.get_int() != begin()");
            abort();
        }

        // ++ operator works
        for (unsigned int i=1; i<ibp.end(); ++i) {
            ++ibp;
            LOG4CXX_DEBUG(ibp.logger, "i: " << i << "; ib: " << ibp);
            if (ibp.get_base() != ibp.int_to_base(i)) {
                LOG4CXX_FATAL(ibp.logger, "ib.get_base() != int_to_base(i)");
                abort();
            }
        }
        LOG4CXX_DEBUG(ibp.logger, "After ++ loop, ib is: " << ibp);

        // -- operator works
        for (unsigned int i=ibp.alphabet_size; i>0; --i) {
            LOG4CXX_DEBUG(ibp.logger, "i: " << i << "; ib: " << ibp);
            if (ibp.get_base() != ibp.int_to_base(i-1)) {
                LOG4CXX_FATAL(ibp.logger, "ib.get_base() != int_to_base(i-1)");
                abort();
            }
            --ibp;
        }
        LOG4CXX_DEBUG(ibp.logger, "After -- loop, ib is: " << ibp);
    };
    friend std::ostream& operator<< (std::ostream &stream, intbase ib) {
        stream << "{" << std::dec << ib.get_base() << " (" << ib.get_int() << ")}";
        return stream;
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
