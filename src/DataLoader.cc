#include "DataLoader.h"
#include "Config.h"
#include "Utils.h"
#include <iostream>
#include <cstdlib>

DataLoader::DataLoader(const std::string& filename, const std::string& treeName) {
    file_.reset(TFile::Open(filename.c_str(), "READ"));
    if (!file_ || file_->IsZombie()) {
        std::cerr << "ERROR: Cannot open file " << filename << std::endl;
        std::exit(1);
    }
    tree_ = dynamic_cast<TTree*>(file_->Get(treeName.c_str()));
    if (!tree_) {
        std::cerr << "ERROR: Cannot find TTree '" << treeName << "' in " << filename << std::endl;
        std::exit(1);
    }
    // Disable all branches by default, enable only what we need
    tree_->SetBranchStatus("*", 0);
}

DataLoader::~DataLoader() = default;

void DataLoader::setupBranches(EventData& evt) {
    auto on = [&](const char* name) { tree_->SetBranchStatus(name, 1); };

    // Event IDs
    on("run");   tree_->SetBranchAddress("run",   &evt.run);
    on("event"); tree_->SetBranchAddress("event", &evt.event);
    on("lumi");  tree_->SetBranchAddress("lumi",  &evt.lumi);

    // Weights
    on("weight");        tree_->SetBranchAddress("weight",        &evt.weight);
    on("eventWeight");   tree_->SetBranchAddress("eventWeight",   &evt.eventWeight);
    on("weight_central"); tree_->SetBranchAddress("weight_central", &evt.weight_central);

    // Diphoton kinematics
    on("mass"); tree_->SetBranchAddress("mass", &evt.mass);
    on("pt");   tree_->SetBranchAddress("pt",   &evt.pt);
    on("eta");  tree_->SetBranchAddress("eta",  &evt.eta);
    on("phi");  tree_->SetBranchAddress("phi",  &evt.phi);

    // Lead photon
    on("lead_pt");     tree_->SetBranchAddress("lead_pt",     &evt.lead_pt);
    on("lead_eta");    tree_->SetBranchAddress("lead_eta",    &evt.lead_eta);
    on("lead_phi");    tree_->SetBranchAddress("lead_phi",    &evt.lead_phi);
    on("lead_mvaID");  tree_->SetBranchAddress("lead_mvaID",  &evt.lead_mvaID);
    on("lead_r9");     tree_->SetBranchAddress("lead_r9",     &evt.lead_r9);

    // Sublead photon
    on("sublead_pt");    tree_->SetBranchAddress("sublead_pt",    &evt.sublead_pt);
    on("sublead_eta");   tree_->SetBranchAddress("sublead_eta",   &evt.sublead_eta);
    on("sublead_phi");   tree_->SetBranchAddress("sublead_phi",   &evt.sublead_phi);
    on("sublead_mvaID"); tree_->SetBranchAddress("sublead_mvaID", &evt.sublead_mvaID);
    on("sublead_r9");    tree_->SetBranchAddress("sublead_r9",    &evt.sublead_r9);

    // Category flags
    on("is_nonRes");              tree_->SetBranchAddress("is_nonRes",              &evt.is_nonRes);
    on("is_nonResReg");           tree_->SetBranchAddress("is_nonResReg",           &evt.is_nonResReg);
    on("is_nonResReg_DNNpair");   tree_->SetBranchAddress("is_nonResReg_DNNpair",   &evt.is_nonResReg_DNNpair);
    on("is_nonResReg_vbfpair");   tree_->SetBranchAddress("is_nonResReg_vbfpair",   &evt.is_nonResReg_vbfpair);
    on("is_Res");                 tree_->SetBranchAddress("is_Res",                 &evt.is_Res);
    on("is_Res_DNNpair");         tree_->SetBranchAddress("is_Res_DNNpair",         &evt.is_Res_DNNpair);

    // Multiplicities
    on("n_jets");   tree_->SetBranchAddress("n_jets",   &evt.n_jets);
    on("nBLoose");  tree_->SetBranchAddress("nBLoose",  &evt.nBLoose);
    on("nBMedium"); tree_->SetBranchAddress("nBMedium", &evt.nBMedium);
    on("nBTight");  tree_->SetBranchAddress("nBTight",  &evt.nBTight);

    // BDT outputs (Float)
    on("MultiBDT_output_0"); tree_->SetBranchAddress("MultiBDT_output_0", &evt.MultiBDT_output[0]);
    on("MultiBDT_output_1"); tree_->SetBranchAddress("MultiBDT_output_1", &evt.MultiBDT_output[1]);
    on("MultiBDT_output_2"); tree_->SetBranchAddress("MultiBDT_output_2", &evt.MultiBDT_output[2]);
    on("MultiBDT_output_3"); tree_->SetBranchAddress("MultiBDT_output_3", &evt.MultiBDT_output[3]);

    // Discriminants (Float)
    on("alpha");  tree_->SetBranchAddress("alpha",  &evt.alpha);
    on("beta");   tree_->SetBranchAddress("beta",   &evt.beta);
    on("gamma");  tree_->SetBranchAddress("gamma",  &evt.gamma);
    on("D_ttH");  tree_->SetBranchAddress("D_ttH",  &evt.D_ttH);
    on("D_qcd");  tree_->SetBranchAddress("D_qcd",  &evt.D_qcd);

    // MET
    on("puppiMET_pt");  tree_->SetBranchAddress("puppiMET_pt",  &evt.puppiMET_pt);
    on("puppiMET_phi"); tree_->SetBranchAddress("puppiMET_phi", &evt.puppiMET_phi);

    // Sigma m
    on("sigma_m_over_m"); tree_->SetBranchAddress("sigma_m_over_m", &evt.sigma_m_over_m);
}

void DataLoader::setupSchemeBranches(SchemeData& sd, const std::string& schemeKey) {
    const auto& schemes = getSchemes();
    auto it = schemes.find(schemeKey);
    if (it == schemes.end()) {
        std::cerr << "ERROR: Unknown scheme '" << schemeKey << "'" << std::endl;
        return;
    }
    const std::string& p = it->second.prefix;

    auto setup = [&](const std::string& suffix, double* addr) {
        std::string bname = schemeBranch(p, suffix);
        tree_->SetBranchStatus(bname.c_str(), 1);
        tree_->SetBranchAddress(bname.c_str(), addr);
    };

    // Dijet
    setup("dijet_mass",          &sd.dijet_mass);
    setup("dijet_pt",            &sd.dijet_pt);
    setup("dijet_eta",           &sd.dijet_eta);
    setup("dijet_mass_DNNreg",   &sd.dijet_mass_DNNreg);

    // Lead b-jet
    setup("lead_bjet_pt",              &sd.lead_bjet_pt);
    setup("lead_bjet_eta",             &sd.lead_bjet_eta);
    setup("lead_bjet_phi",             &sd.lead_bjet_phi);
    setup("lead_bjet_mass",            &sd.lead_bjet_mass);
    setup("lead_bjet_btagPNetB",       &sd.lead_bjet_btagPNetB);
    setup("lead_bjet_btagUParTAK4B",   &sd.lead_bjet_btagUParTAK4B);

    // Sublead b-jet
    setup("sublead_bjet_pt",            &sd.sublead_bjet_pt);
    setup("sublead_bjet_eta",           &sd.sublead_bjet_eta);
    setup("sublead_bjet_phi",           &sd.sublead_bjet_phi);
    setup("sublead_bjet_mass",          &sd.sublead_bjet_mass);
    setup("sublead_bjet_btagPNetB",     &sd.sublead_bjet_btagPNetB);
    setup("sublead_bjet_btagUParTAK4B", &sd.sublead_bjet_btagUParTAK4B);

    // HH candidate
    setup("HHbbggCandidate_mass", &sd.HHbbggCandidate_mass);
    setup("HHbbggCandidate_pt",   &sd.HHbbggCandidate_pt);

    // Angular / kinematic
    setup("CosThetaStar_CS",  &sd.CosThetaStar_CS);
    setup("DeltaR_jg_min",    &sd.DeltaR_jg_min);
    setup("M_X",              &sd.M_X);
    setup("chi_t0",           &sd.chi_t0);
    setup("chi_t1",           &sd.chi_t1);

    // Photon pT / mgg
    setup("pholead_PtOverM",    &sd.pholead_PtOverM);
    setup("phosublead_PtOverM", &sd.phosublead_PtOverM);

    // Flag
    setup("has_two_btagged_jets", &sd.has_two_btagged_jets);
}

Long64_t DataLoader::getEntries() const {
    return tree_->GetEntries();
}

void DataLoader::getEntry(Long64_t i) {
    tree_->GetEntry(i);
}
