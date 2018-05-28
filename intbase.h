#ifndef INTBASE_H
#define INTBASE_H

/*! @file intbase.h
 *  @brief A sequence base stored as an integer and utilities to work with it.
 * This class *must* be subclassed.
 */

#include <algorithm>
#include <string>
#include <log4cxx/logger.h>

/*! @class base
 * @brief a class to hold the knowledge about mapping sequence bases to integers and vice versa
 */

const std::string endmarker = ">";

typedef std::string base_t;
typedef std::vector<base_t> bases_t;
static bases_t realbases;   //!< All legal bases plus end marker

typedef std::map<base_t, unsigned int> mapping_t;
static mapping_t realmap;

class base {
protected:
    // !!! bases *must* be initiaized in a subclass.
    bases_t &bases = realbases;
    unsigned int alphabet_size = 0; //!< Number of bases we work with
    unsigned int base_bitmask;  //!< bitmask for a single base in an integer
    unsigned int base_nbits;    //!< Number of bits used to store a base
    mapping_t &mapping = realmap; //!< character to base value mapping.  Must match bases

    //! The base value
    unsigned int base_value;

    // Everything depends on the length of bases
    void set_consts() {
        alphabet_size = bases.size()-1;
        base_nbits = 0;
        base_bitmask = 0;
        unsigned int t = alphabet_size;
        while (t >>= 1) {
            base_bitmask |= 1 << base_nbits;
            ++base_nbits;
        }
        for (unsigned int i=0; i< bases.size(); ++i) {
            mapping.insert(std::pair<base_t, unsigned int>(bases[i], i));
        }
    };

    // logging
    log4cxx::LoggerPtr logger = nullptr;
    void init_logging(void) {
        logger = log4cxx::Logger::getLogger("base");
    };

public:
    base() {
        // set_consts must be called by subclass
        init_logging();
        base_value = begin(); // initialized to first legal value unless via a constructor with an initial value.
    };

    unsigned int get_nbits() const {
        return base_nbits;
    };
    unsigned int get_bitmask() const {
        return base_bitmask;
    };
    unsigned int get_alphabetsize() const {
        if (alphabet_size == 0) {
            LOG4CXX_FATAL(logger, "alphabet_size is 0!");
            abort();
        }
        return alphabet_size;
    }
    unsigned int get_int() const {
        return base_value;
    };
    base_t get_base() const {
        return bases[base_value];
    };
    void set_base(unsigned int b) {
        if (b < get_alphabetsize()) {
            base_value = b;
        } else {
            LOG4CXX_FATAL(logger, "base " << b << " >= alphabet size " << get_alphabetsize());
            abort();
        }
    };
    void set_base(const base_t b) {
        set_base(base_to_int(b));
    };
    unsigned int base_to_int(const base_t base_p) {
        std::string b = base_p; // So we can make it uppercase
        std::transform(b.begin(), b.end(), b.begin(), ::toupper); // ensure b is all uc
        for (unsigned int i=0; i<get_alphabetsize(); ++i) {
            if (b.compare(bases[i]) == 0)
                return i;
        }
        log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("base"));
        LOG4CXX_FATAL(logger, "base_to_int: unknown base '" << b << "'");
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
    base_t int_to_base(unsigned int value) {
        if (value < get_alphabetsize()) {
            return bases[value];
        } else if (value == get_alphabetsize()) {
            return endmarker;
        } else { // fatal error
            log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("base"));

            LOG4CXX_FATAL(logger, "int_to_base: invalid base value: " << value << " (max " << alphabet_size << ")");
            abort();
        }
        /*NOTREACHED*/
        return 0;
    };

    base& operator=(const base &i) {
        if (this == &i)      // Same object?
            return *this;
        base_value = i.base_value;
        return *this;
    };
    base& operator=(const unsigned int i) {
        if (i >= alphabet_size) {
            LOG4CXX_FATAL(logger, "i >= alphabet_size");
            abort();
        }
        base_value = i;
        return *this;
    };
    base& operator++(void) { // Allowed to equal base::end()
        if (base_value < get_alphabetsize())
            ++base_value;
        else if (base_value == get_alphabetsize()) {
            // do nothing; we are at the end and cannot increment more
        }
        else { // base > get_alphabetsize(); we should never be here
            LOG4CXX_FATAL(logger, "base ++ on too-large value!");
            abort();
        }
        return *this;
    };
    base& operator--(void) {
        if (base_value > 0)
            --base_value;
        else // base == alphabet_size-1
            base_value = alphabet_size-1;
        return *this;
    };
    bool operator<(const base& rhs) const {
        return base_value < rhs.get_int();
    };
    bool operator<(const unsigned int rhs) const {
        return base_value < rhs;
    };
    bool operator>(const base& rhs) const {
        return base_value > rhs.get_int();
    };
    bool operator==(const base& rhs) const {
        return base_value == rhs.get_int();
    };
    bool operator!=(const base& rhs) const {
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

    //! @brief Unit test for base class
    //! subclasses need their own tests specific to the subclass
    //! ibp must be a freshly-created instance
    friend void test_base(base& ibp) {
        if (ibp.alphabet_size == 0) {
            LOG4CXX_FATAL(ibp.logger, "test_base must be called on a subclass!");
            abort();
        }

        if (ibp.bases.size() != ibp.get_alphabetsize()+1) {
            LOG4CXX_FATAL(ibp.logger, "number of bases " << ibp.bases.size() << " does not equal alphabet_size(" << ibp.get_alphabetsize() << ") + 1 !");
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

            base_t b = ibp.int_to_base(i);
            unsigned int ui = ibp.base_to_int(b);
            if (ui != i) {
                LOG4CXX_FATAL(ibp.logger, "base_to_int(bases[i]) != i");
                abort();
            }

            LOG4CXX_TRACE(ibp.logger, "i: " << std::dec << i << " converts to '" << b << "'.");

            b = ibp.bases.at(i);
            ui = ibp.base_to_int(b);
            if (ui != i) {
                LOG4CXX_FATAL(ibp.logger, "ui != i");
                abort();
            }

            LOG4CXX_TRACE(ibp.logger, "base '" << b << "' converts to " << std::dec << ui << ".");
        }
        LOG4CXX_INFO(ibp.logger, "base_to_int and int_to_base work OK.");

        LOG4CXX_DEBUG(ibp.logger, "Newly-created base: " << ibp);

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
    friend std::ostream& operator<< (std::ostream &stream, base ib) {
        stream << "{" << std::dec << ib.get_base() << " (" << ib.get_int() << ")}";
        return stream;
    };
};

namespace std
{
template <>
struct hash<base>
{
    size_t operator()(const base& ib) const
    {
        return hash<unsigned int>()(ib.get_int());
    }
};
}


#endif // INTBASE_H
