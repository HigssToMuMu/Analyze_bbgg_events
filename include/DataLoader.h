#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <memory>
#include <TFile.h>
#include <TTree.h>

// Common event-level variables (scheme-independent)
struct EventData {
    // Event identifiers
    unsigned int  run   = 0;
    unsigned long long event = 0;
    unsigned int  lumi  = 0;

    // Weights
    double weight       = 1.0;
    double eventWeight  = 1.0;
    double weight_central = 1.0;

    // Diphoton candidate kinematics
    double mass = 0, pt = 0, eta = 0, phi = 0;

    // Photon variables
    double lead_pt = 0, lead_eta = 0, lead_phi = 0, lead_mvaID = 0, lead_r9 = 0;
    double sublead_pt = 0, sublead_eta = 0, sublead_phi = 0, sublead_mvaID = 0, sublead_r9 = 0;

    // Category flags (stored as Double in the ntuple)
    double is_nonRes = 0, is_nonResReg = 0, is_nonResReg_DNNpair = 0;
    double is_nonResReg_vbfpair = 0, is_Res = 0, is_Res_DNNpair = 0;

    // Multiplicities
    double n_jets = 0, nBLoose = 0, nBMedium = 0, nBTight = 0;

    // BDT outputs (Float in ntuple)
    float MultiBDT_output[4] = {0, 0, 0, 0};

    // Discriminants (Float in ntuple)
    float alpha = 0, beta = 0, gamma = 0, D_ttH = 0, D_qcd = 0;

    // MET
    double puppiMET_pt = 0, puppiMET_phi = 0;

    // Sigma m
    double sigma_m_over_m = 0;
};

// Per-scheme variables (prefix-dependent)
struct SchemeData {
    // Dijet
    double dijet_mass = 0, dijet_pt = 0, dijet_eta = 0;
    double dijet_mass_DNNreg = 0;

    // Lead b-jet
    double lead_bjet_pt = 0, lead_bjet_eta = 0, lead_bjet_phi = 0, lead_bjet_mass = 0;
    double lead_bjet_btagPNetB = 0, lead_bjet_btagUParTAK4B = 0;

    // Sublead b-jet
    double sublead_bjet_pt = 0, sublead_bjet_eta = 0, sublead_bjet_phi = 0, sublead_bjet_mass = 0;
    double sublead_bjet_btagPNetB = 0, sublead_bjet_btagUParTAK4B = 0;

    // HH candidate
    double HHbbggCandidate_mass = 0, HHbbggCandidate_pt = 0;

    // Angular / kinematic
    double CosThetaStar_CS = 0;
    double DeltaR_jg_min = 0;
    double M_X = 0;
    double chi_t0 = 0, chi_t1 = 0;

    // Photon pT / mgg (scheme-level)
    double pholead_PtOverM = 0, phosublead_PtOverM = 0;

    // Flag
    double has_two_btagged_jets = 0;
};

class DataLoader {
public:
    DataLoader(const std::string& filename, const std::string& treeName = "data");
    ~DataLoader();

    void setupBranches(EventData& evt);
    void setupSchemeBranches(SchemeData& sd, const std::string& schemeKey);

    Long64_t getEntries() const;
    void getEntry(Long64_t i);
    TTree* getTree() const { return tree_; }

private:
    std::unique_ptr<TFile> file_;
    TTree* tree_ = nullptr; // owned by TFile
};

#endif
