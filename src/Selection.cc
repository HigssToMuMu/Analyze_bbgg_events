#include "Selection.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <cmath>

EventSelector::EventSelector(const SelectionCuts& cuts) : cuts_(cuts) {}

bool EventSelector::passDiphotonMass(const EventData& evt) const {
    return evt.mass >= cuts_.mggMin && evt.mass <= cuts_.mggMax;
}

bool EventSelector::passPhotonPt(const EventData& evt) const {
    if (evt.mass <= 0) return false;
    return (evt.lead_pt / evt.mass) > cuts_.leadPtOverMgg &&
           (evt.sublead_pt / evt.mass) > cuts_.subleadPtOverMgg;
}

bool EventSelector::passPhotonMvaId(const EventData& evt) const {
    return evt.lead_mvaID > cuts_.mvaIdMin && evt.sublead_mvaID > cuts_.mvaIdMin;
}

bool EventSelector::passDijetMass(const SchemeData& sd) const {
    if (isSentinel(sd.dijet_mass)) return false;
    return sd.dijet_mass >= cuts_.mjjMin && sd.dijet_mass <= cuts_.mjjMax;
}

bool EventSelector::passBjetPt(const SchemeData& sd) const {
    if (isSentinel(sd.lead_bjet_pt)) return false;
    if (isSentinel(sd.sublead_bjet_pt)) return false;
    return sd.lead_bjet_pt > cuts_.bjetPtMin && sd.sublead_bjet_pt > cuts_.bjetPtMin;
}

bool EventSelector::passBtagMultiplicity(const EventData& evt) const {
    return static_cast<int>(evt.nBLoose) >= cuts_.nBLooseMin;
}

bool EventSelector::passSchemeFlag(const EventData& evt, const std::string& schemeKey) const {
    if (schemeKey == "nonRes")              return evt.is_nonRes > 0.5;
    if (schemeKey == "nonResReg")           return evt.is_nonResReg > 0.5;
    if (schemeKey == "nonResReg_DNNpair")   return evt.is_nonResReg_DNNpair > 0.5;
    if (schemeKey == "nonResReg_vbfpair")   return evt.is_nonResReg_vbfpair > 0.5;
    if (schemeKey == "Res")                 return evt.is_Res > 0.5;
    if (schemeKey == "Res_DNNpair")         return evt.is_Res_DNNpair > 0.5;
    return false;
}

bool EventSelector::passSideband(const EventData& evt) const {
    return evt.mass < BLIND_LOW || evt.mass > BLIND_HIGH;
}

bool EventSelector::passSignalRegion(const EventData& evt) const {
    return evt.mass >= BLIND_LOW && evt.mass <= BLIND_HIGH;
}

bool EventSelector::passPreselection(const EventData& evt, const SchemeData& sd,
                                     const std::string& schemeKey) const {
    if (!passSchemeFlag(evt, schemeKey)) return false;
    if (!passDiphotonMass(evt))         return false;
    if (!passPhotonPt(evt))             return false;
    if (!passPhotonMvaId(evt))          return false;
    if (!passDijetMass(sd))             return false;
    if (!passBjetPt(sd))                return false;
    return true;
}

void EventSelector::printCutflow(DataLoader& loader, const std::string& schemeKey) const {
    const auto& schemes = getSchemes();
    auto it = schemes.find(schemeKey);
    if (it == schemes.end()) {
        std::cerr << "ERROR: Unknown scheme '" << schemeKey << "' for cutflow" << std::endl;
        return;
    }

    // Set up a temporary SchemeData for this cutflow
    EventData evt;
    SchemeData sd;
    loader.setupBranches(evt);
    loader.setupSchemeBranches(sd, schemeKey);

    struct CutInfo {
        std::string label;
        int count = 0;
    };
    std::vector<CutInfo> cutflow = {
        {"Total events"},
        {"Scheme flag (" + schemeKey + ")"},
        {"m_{gg} in [" + std::to_string((int)cuts_.mggMin) + "," + std::to_string((int)cuts_.mggMax) + "]"},
        {"Photon pT/m_{gg}"},
        {"Photon MVA ID > " + std::to_string(cuts_.mvaIdMin).substr(0, 5)},
        {"m_{jj} in [" + std::to_string((int)cuts_.mjjMin) + "," + std::to_string((int)cuts_.mjjMax) + "]"},
        {"b-jet pT > " + std::to_string((int)cuts_.bjetPtMin) + " GeV"},
    };

    Long64_t nEntries = loader.getEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        loader.getEntry(i);

        int step = 0;
        cutflow[step++].count++; // Total

        if (!passSchemeFlag(evt, schemeKey)) continue;
        cutflow[step++].count++;

        if (!passDiphotonMass(evt)) continue;
        cutflow[step++].count++;

        if (!passPhotonPt(evt)) continue;
        cutflow[step++].count++;

        if (!passPhotonMvaId(evt)) continue;
        cutflow[step++].count++;

        if (!passDijetMass(sd)) continue;
        cutflow[step++].count++;

        if (!passBjetPt(sd)) continue;
        cutflow[step++].count++;
    }

    // Print table
    std::cout << "\n===== Cutflow: " << it->second.name << " (" << schemeKey << ") =====" << std::endl;
    std::cout << std::left << std::setw(45) << "Cut"
              << std::right << std::setw(10) << "Events"
              << std::setw(12) << "Eff (%)" << std::endl;
    std::cout << std::string(67, '-') << std::endl;

    int total = cutflow[0].count;
    for (auto& c : cutflow) {
        double eff = (total > 0) ? 100.0 * c.count / total : 0.0;
        std::cout << std::left << std::setw(45) << c.label
                  << std::right << std::setw(10) << c.count
                  << std::setw(11) << std::fixed << std::setprecision(1) << eff << "%" << std::endl;
    }
    std::cout << std::endl;
}
