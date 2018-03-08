// Interface for all measure functions

#ifndef MEASURE_H
#define MEASURE_H

#include "fasta.h"

class measure {
    public:
	virtual void init() {};
	virtual void printdetails() {};

	// needs to be implemented in subclasses; -1 is illegal for a distance.
	virtual long double compare(const FastaRecord& a, const FastaRecord& b) const {return -1.0;};

	// Not needed?
	//virtual measure& operator=(const measure &c) {};

	//virtual ~measure() {};
};

#endif // MEASURE_H
