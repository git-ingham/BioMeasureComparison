#ifndef EDITMETRIC_H
#define EDITMETRIC_H

#include "metric.h"
#include "editcost.h"

#include <boost/algorithm/sequence/edit_distance.hpp>
#include <algorithm>

using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;

class editmetric : public metric {
    editcost cost;
    custom_cost_s custom_cost;
    bool use_cost = false;

    public:
	editmetric(std::string costfname);
	~editmetric() {};

	long double compare(const FastaRecord& a, const FastaRecord& b) const;

	void printdetails() {
	    std::cerr << "Levenshtein distance:" << std::endl;
	    if (use_cost) 
	        cost.print();
	    else 
	        std::cerr << "    Unit cost for all operations." << std::endl;
	};
};
#endif // EDITMETRIC_H
