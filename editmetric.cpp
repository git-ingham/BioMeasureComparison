#include "editmetric.h"
#include <boost/algorithm/sequence/edit_distance.hpp>
#include <algorithm>

using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;

long double
editmetric::compare(const FastaRecord& a, const FastaRecord& b) const {
    int d = edit_distance(a.get_seq(), b.get_seq());
    //return (double)d / std::max(a.get_seq().length(), b.get_seq().length());
    return (long double)d;
};
