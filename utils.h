#ifndef UTILS_H
#define UTILS_H

#include <string>

bool checkdir(std::string checkpointdir);
void chomp(std::string& line);
bool checkmakedir(std::string checkpointdir);
bool fileexists(std::string fname);

#endif // UTILS_H
