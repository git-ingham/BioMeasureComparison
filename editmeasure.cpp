#include "editmeasure.h"

#include <boost/algorithm/sequence/edit_distance.hpp>
#include <algorithm>
#include <err.h>
#include <ctype.h>

editmeasure::editmeasure(std::string costfname)
{
    if (costfname.length() > 0) {
        cost.init(costfname);
        custom_cost = cost.custom_cost();
        use_cost = true;
    }
}

using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;

long double
editmeasure::compare(const FastaRecord& a, const FastaRecord& b) {
    if (use_cost)
        return edit_distance(a.get_seq(), b.get_seq(), custom_cost);
    else
        return edit_distance(a.get_seq(), b.get_seq());
}

void 
editmeasure::printdetails(void)
{
    std::cerr << "Levenshtein distance:" << std::endl;
    if (use_cost)
        cost.print();
    else
        std::cerr << "    Unit cost for all operations." << std::endl;
}
