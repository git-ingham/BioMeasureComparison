#ifndef INTBASEDNA_H
#define INTBASEDNA_H

#include "intbase.h"
#include <string>

class intbaseDNA : public intbase {
    void set_consts() {
        bases = "ACGT>";
        intbase::set_consts();
    };

public:
    intbaseDNA() : intbase() {
        set_consts();
    };
    intbaseDNA(const char b) : intbase() {
        set_consts();
        intbase::init_logging();
        base = base_to_int(b);
    };
    intbaseDNA(const unsigned int b) : intbase() {
        set_consts();
        if (b <= get_alphabetsize())
            base = b;
        else {
            LOG4CXX_FATAL(logger, "intbaseDNA constructor: Invalid int base value b " << b << " should be in [0.." << get_alphabetsize() << "].  " << get_alphabetsize() << " is invalid, but is the end indicator.");
            abort();
        }
    };
    intbaseDNA(const intbaseDNA &ib) : intbase() {
        set_consts();
        base = ib.base;
    };
    friend void test_intbaseDNA(intbaseDNA& ib) {
        // >, <, ==, and != operators work
        ib.set_base((unsigned int)1);
        intbaseDNA ib2;
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

#endif // INTBASEDNA_H
