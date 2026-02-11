#ifndef SELECTION_H
#define SELECTION_H

#include "Config.h"
#include "DataLoader.h"
#include <string>
#include <vector>
#include <map>

class EventSelector {
public:
    explicit EventSelector(const SelectionCuts& cuts = SelectionCuts{});

    // Individual cut methods
    bool passDiphotonMass(const EventData& evt) const;
    bool passPhotonPt(const EventData& evt) const;
    bool passPhotonMvaId(const EventData& evt) const;
    bool passDijetMass(const SchemeData& sd) const;
    bool passBjetPt(const SchemeData& sd) const;
    bool passBtagMultiplicity(const EventData& evt) const;
    bool passSchemeFlag(const EventData& evt, const std::string& schemeKey) const;
    bool passSideband(const EventData& evt) const;
    bool passSignalRegion(const EventData& evt) const;

    // Combined preselection
    bool passPreselection(const EventData& evt, const SchemeData& sd,
                          const std::string& schemeKey) const;

    // Cutflow: runs its own event loop, prints table
    void printCutflow(DataLoader& loader, const std::string& schemeKey) const;

    const SelectionCuts& getCuts() const { return cuts_; }

private:
    SelectionCuts cuts_;
};

#endif
