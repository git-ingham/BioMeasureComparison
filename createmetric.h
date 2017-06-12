#ifndef CREATEMETRIC_H
#define CREATEMETRIC_H

#include <string>
#include "metric.h"

metric* 
createmetric(const std::string& name, const std::string& subname, const std::string& opts, const fastavec_t& seqs);

#endif // CREATEMETRIC_H
