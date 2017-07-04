#ifndef CREATEMETRIC_H
#define CREATEMETRIC_H

#include <string>
#include "metric.h"
#include "Options.h"
#include <iostream>
#include "editmetric.h"
#include "kmermetric.h"
#include "Options.h"

metric* 
createmetric(const Options opts, const fastavec_t& seqs);
//createmetric(const std::string& name, const std::string& subname, const std::string& opts, const fastavec_t& seqs);

#endif // CREATEMETRIC_H
