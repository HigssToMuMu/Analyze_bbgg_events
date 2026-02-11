#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string schemeBranch(const std::string& prefix, const std::string& suffix);
void ensureDirectory(const std::string& path);
bool isSentinel(double val, double sentinel = -999.0);

#endif
