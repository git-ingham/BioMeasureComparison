// Interface for all metric functions

#ifndef METRIC_H
#define METRIC_H

#include "fasta.h"

class metric {
    public:
	virtual void init() {};
	virtual void printdetails() {};

	// needs to be implemented in subclasses; -1 is illegal for a distance.
	virtual long double compare(const FastaRecord& a, const FastaRecord& b) {return -1.0;};

	// Not needed?
	//virtual metric& operator=(const metric &c) {};

	//virtual ~metric() {};
};

#endif // METRIC_H
