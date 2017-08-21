#ifndef UTILS_H
#define UTILS_H

#include <string>

void chomp(std::string& line);
bool checkmakedir(std::string dir);
bool fileexists(std::string fname);
bool direxists(std::string dir);

#endif // UTILS_H
