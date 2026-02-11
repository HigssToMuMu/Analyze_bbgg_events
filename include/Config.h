#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <vector>

// Physics constants
constexpr double HIGGS_MASS   = 125.0;
constexpr double BLIND_LOW    = 115.0;
constexpr double BLIND_HIGH   = 135.0;
constexpr double SENTINEL     = -999.0;
constexpr double LUMI_RUN3    = 61.9; // fb^-1
constexpr double SQRT_S       = 13.6; // TeV

struct JetPairingScheme {
    std::string name;           // display name
    std::string prefix;         // branch prefix (e.g. "nonRes_")
    std::string categoryFlag;   // branch name for is_XXX flag
    bool isResonant;
    bool hasVbfBranches;
};

struct SelectionCuts {
    double leadPtOverMgg    = 1.0 / 3.0;
    double subleadPtOverMgg = 1.0 / 4.0;
    double mvaIdMin         = -0.7;
    double mggMin           = 100.0;
    double mggMax           = 180.0;
    double mjjMin           = 70.0;
    double mjjMax           = 190.0;
    double bjetPtMin        = 25.0;
    int    nBLooseMin       = 1;
};

struct PlotDef {
    int nbins;
    double xmin;
    double xmax;
    std::string xlabel;
    std::string units;
};

const std::map<std::string, JetPairingScheme>& getSchemes();
std::map<std::string, PlotDef> getPlotDefs();
std::map<std::string, PlotDef> getSchemePlotDefs();

#endif
