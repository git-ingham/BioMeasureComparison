#ifndef INTBASEDNA_H
#define INTBASEDNA_H

#include "intbase.h"
#include <string>

const unsigned int nDNAbases = 5;
std::string DNAbases[nDNAbases] = {"A", "C", "G", "T", endmarker};

class intbaseDNA : public intbase {
    void set_consts() {
        bases.clear(); // WARNING: not a good idea to to use multiple intbase subclasses simultaneously!
        for (unsigned int i=0; i<nDNAbases; ++i) {
            bases.push_back(DNAbases[i]);
        }
        intbase::set_consts();
    };

public:
    intbaseDNA() : intbase() {
        set_consts();
    };
    intbaseDNA(base_t b) : intbase() {
        set_consts();
        intbase::init_logging();
        base_value = base_to_int(b);
    };
    intbaseDNA(const unsigned int b) : intbase() {
        set_consts();
        if (b <= get_alphabetsize())
            base_value = b;
        else {
            LOG4CXX_FATAL(logger, "intbaseDNA constructor: Invalid int base value b " << b << " should be in [0.." << get_alphabetsize() << "].  " << get_alphabetsize() << " is invalid, but is the end indicator.");
            abort();
        }
    };
    intbaseDNA(const intbaseDNA &ib) : intbase() {
        set_consts();
        base_value = ib.base_value;
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
