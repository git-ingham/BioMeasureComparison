#ifndef EDITMETRIC_H
#define EDITMETRIC_H

#include "metric.h"

#include <boost/algorithm/sequence/edit_distance.hpp>
#include <algorithm>

using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;

class editmetric : public metric {
    public:
	void init() {};
	void printdetails() {
	    std::cerr << "Levenshtein distance:" << std::endl;
	    std::cerr << "    unit cost per operation" << std::endl;
	};

	//### probably need a cost function for a separate test
	//### this can come in through the init function.

	long double compare(const FastaRecord& a, const FastaRecord& b) const;
    
	//metric& operator=(const metric &c) {return *this;};

	~editmetric() {};
};

#endif // EDITMETRIC_H
