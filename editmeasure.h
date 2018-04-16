#ifndef EDITMEASURE_H
#define EDITMEASURE_H

#include "measure.h"
#include "editcost.h"

#include <boost/algorithm/sequence/edit_distance.hpp>
#include <algorithm>

using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;

class editmeasure : public measure {
    editcost cost;
    custom_cost_s custom_cost;
    bool use_cost = false;

    public:
	editmeasure(std::string costfname);
	~editmeasure() {};

	long double compare(const FastaRecord& a, const FastaRecord& b);

	void printdetails(void);
    void test() {}; //!< @todo implement this
};
#endif // EDITMEASURE_H
