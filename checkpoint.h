#pragma once 

#include "Options.h"

void workercheckpoint(unsigned int i, unsigned int workernum, std::string checkpointdir);
unsigned int workerrestore(unsigned int workernum, std::string checkpointdir);
std::string restoreoption(const std::string label, std::ifstream& cpf);
