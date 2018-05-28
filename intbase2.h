#ifndef INTBASE2_H
#define INTBASE2_H

/*
 * For debugging to simplify the deBruijn graph, a minimal set of bases
 */

#include "intbase.h"

const unsigned int n2bases = 3;
std::string bases2[n2bases] = {"A", "C", endmarker};

class intbase2 : public intbase {
    void set_consts() {
        bases.clear(); // WARNING: not a good idea to to use multiple intbase subclasses!
        for (unsigned int i=0; i<n2bases; ++i) {
            bases.push_back(bases2[i]);
        }
        intbase::set_consts();
    };

public:
    intbase2() : intbase() {
        set_consts();
    };
    intbase2(base_t b) : intbase() {
        set_consts();
        intbase::init_logging();
        base_value = base_to_int(b);
    };
    intbase2(const unsigned int b) : intbase() {
        set_consts();
        if (b <= get_alphabetsize())
            base_value = b;
        else {
            LOG4CXX_FATAL(logger, "intbase2 constructor: Invalid int base value b " << b << " should be in [0.." << get_alphabetsize() << "].  " << get_alphabetsize() << " is invalid, but is the end indicator.");
            abort();
        }
    };
    intbase2(const intbase2 &ib) : intbase() {
        set_consts();
        base_value = ib.base_value;
    };
    friend void test_intbase2(intbase2& ib) {
        // >, <, ==, and != operators work
        ib.set_base((unsigned int)1);
        intbase2 ib2;
        ib2.set_base((unsigned int)0);
        if (!(ib2 < ib)) {
            LOG4CXX_FATAL(ib.logger, "ib2 " << ib2 << " >= ib " << ib);
            abort();
        }
        if (!(ib > ib2)) {
            LOG4CXX_FATAL(ib.logger, "ib <= ib2");
            abort();
        }
        if (!(ib == ib)) {
            LOG4CXX_FATAL(ib.logger, "ib != ib");
            abort();
        }
        if (!(ib2 != ib)) {
            LOG4CXX_FATAL(ib.logger, "ib2 == ib");
            abort();
        }
        LOG4CXX_INFO(ib.logger, "Relational operators work.");
    }
};

#endif // INTBASE2_H
