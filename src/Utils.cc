#include "Utils.h"
#include <TSystem.h>
#include <cmath>

std::string schemeBranch(const std::string& prefix, const std::string& suffix) {
    return prefix + suffix;
}

void ensureDirectory(const std::string& path) {
    gSystem->mkdir(path.c_str(), true);
}

bool isSentinel(double val, double sentinel) {
    return std::abs(val - sentinel) < 0.1;
}
