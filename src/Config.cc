#include "Config.h"

const std::map<std::string, JetPairingScheme>& getSchemes() {
    static const std::map<std::string, JetPairingScheme> schemes = {
        {"nonRes",              {"Non-Resonant",                     "nonRes_",              "is_nonRes",              false, true }},
        {"nonResReg",           {"Non-Resonant (Reg)",               "nonResReg_",           "is_nonResReg",           false, false}},
        {"nonResReg_DNNpair",   {"Non-Resonant (Reg, DNN pair)",     "nonResReg_DNNpair_",   "is_nonResReg_DNNpair",   false, true }},
        {"nonResReg_vbfpair",   {"Non-Resonant (Reg, VBF pair)",     "nonResReg_vbfpair_",   "is_nonResReg_vbfpair",   false, true }},
        {"Res",                 {"Resonant",                         "Res_",                 "is_Res",                 true,  false}},
        {"Res_DNNpair",         {"Resonant (DNN pair)",              "Res_DNNpair_",         "is_Res_DNNpair",         true,  false}},
    };
    return schemes;
}

std::map<std::string, PlotDef> getPlotDefs() {
    return {
        // Diphoton
        {"mass",                {80, 100, 180, "m_{#gamma#gamma}",          "GeV"}},
        {"pt",                  {60, 0,   600, "p_{T}^{#gamma#gamma}",     "GeV"}},
        {"eta",                 {50, -5,  5,   "#eta^{#gamma#gamma}",      ""}},
        {"phi",                 {50, -3.15, 3.15, "#phi^{#gamma#gamma}",   ""}},
        // Photons
        {"lead_pt",             {60, 0,   300, "Lead #gamma p_{T}",        "GeV"}},
        {"lead_eta",            {50, -3,  3,   "Lead #gamma #eta",         ""}},
        {"lead_mvaID",          {50, -1,  1,   "Lead #gamma MVA ID",       ""}},
        {"lead_r9",             {50, 0,   1.2, "Lead #gamma R9",           ""}},
        {"sublead_pt",          {60, 0,   200, "Sublead #gamma p_{T}",     "GeV"}},
        {"sublead_eta",         {50, -3,  3,   "Sublead #gamma #eta",      ""}},
        {"sublead_mvaID",       {50, -1,  1,   "Sublead #gamma MVA ID",    ""}},
        {"sublead_r9",          {50, 0,   1.2, "Sublead #gamma R9",        ""}},
        // BDT outputs
        {"MultiBDT_output_0",   {50, 0, 1, "MultiBDT score 0", ""}},
        {"MultiBDT_output_1",   {50, 0, 1, "MultiBDT score 1", ""}},
        {"MultiBDT_output_2",   {50, 0, 1, "MultiBDT score 2", ""}},
        {"MultiBDT_output_3",   {50, 0, 1, "MultiBDT score 3", ""}},
        // Multiplicities
        {"n_jets",              {15, 0, 15, "N_{jets}",         ""}},
        {"nBLoose",             {8,  0, 8,  "N_{b-jets} (Loose)", ""}},
        {"nBMedium",            {8,  0, 8,  "N_{b-jets} (Medium)",""}},
        {"nBTight",             {8,  0, 8,  "N_{b-jets} (Tight)", ""}},
        // MET
        {"puppiMET_pt",         {50, 0, 200, "Puppi MET",        "GeV"}},
        {"puppiMET_phi",        {50, -3.15, 3.15, "Puppi MET #phi", ""}},
        // Sigma m
        {"sigma_m_over_m",      {50, 0, 0.05, "#sigma_{m}/m",   ""}},
        // Discriminants
        {"alpha",               {50, 0, 1,  "#alpha", ""}},
        {"beta",                {50, 0, 1,  "#beta",  ""}},
        {"gamma",               {50, 0, 1,  "#gamma", ""}},
        {"D_ttH",               {50, 0, 1,  "D_{t#bar{t}H}", ""}},
        {"D_qcd",               {50, 0, 1,  "D_{QCD}",       ""}},
    };
}

std::map<std::string, PlotDef> getSchemePlotDefs() {
    return {
        // Dijet
        {"dijet_mass",                  {60, 0, 300, "m_{jj}",                           "GeV"}},
        {"dijet_mass_DNNreg",           {60, 0, 300, "m_{jj} (DNN reg)",                 "GeV"}},
        {"dijet_pt",                    {60, 0, 400, "p_{T}^{jj}",                       "GeV"}},
        // Lead b-jet
        {"lead_bjet_pt",               {60, 0, 300, "Lead b-jet p_{T}",                  "GeV"}},
        {"lead_bjet_eta",              {50, -3, 3,  "Lead b-jet #eta",                   ""}},
        {"lead_bjet_btagPNetB",        {50, 0, 1,   "Lead b-jet PNet B score",           ""}},
        {"lead_bjet_btagUParTAK4B",    {50, 0, 1,   "Lead b-jet UParT AK4 B score",     ""}},
        // Sublead b-jet
        {"sublead_bjet_pt",            {60, 0, 200, "Sublead b-jet p_{T}",               "GeV"}},
        {"sublead_bjet_eta",           {50, -3, 3,  "Sublead b-jet #eta",                ""}},
        {"sublead_bjet_btagPNetB",     {50, 0, 1,   "Sublead b-jet PNet B score",        ""}},
        {"sublead_bjet_btagUParTAK4B", {50, 0, 1,   "Sublead b-jet UParT AK4 B score",  ""}},
        // HH candidate
        {"HHbbggCandidate_mass",       {60, 200, 1400, "m_{bb#gamma#gamma}",             "GeV"}},
        {"HHbbggCandidate_pt",         {60, 0, 500,    "p_{T}^{bb#gamma#gamma}",         "GeV"}},
        // Angular / kinematic
        {"CosThetaStar_CS",            {50, -1, 1,   "cos#theta*_{CS}",                  ""}},
        {"DeltaR_jg_min",              {50, 0, 6,    "#DeltaR_{jg}^{min}",               ""}},
        {"M_X",                        {60, 200, 1400, "M_{X}",                          "GeV"}},
        {"chi_t0",                     {50, 0, 50,   "#chi_{t0}",                        ""}},
        {"chi_t1",                     {50, 0, 50,   "#chi_{t1}",                        ""}},
        {"pholead_PtOverM",            {50, 0, 3,    "Lead #gamma p_{T}/m_{#gamma#gamma}",""}},
        {"phosublead_PtOverM",         {50, 0, 2,    "Sublead #gamma p_{T}/m_{#gamma#gamma}",""}},
    };
}
