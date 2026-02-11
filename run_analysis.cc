#include "Config.h"
#include "DataLoader.h"
#include "Selection.h"
#include "Plotter.h"
#include "Utils.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <TH1D.h>
#include <TH2D.h>

// ---------------------------------------------------------------------------
// CLI argument parsing
// ---------------------------------------------------------------------------
struct CLIArgs {
    std::string input      = "data/all_data_full.root";
    std::string outputDir  = "plots";
    std::vector<std::string> schemes; // empty → all
    bool noBlind           = false;
    bool cutflowOnly       = false;
};

CLIArgs parseArgs(int argc, char** argv) {
    CLIArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--input" && i + 1 < argc)          { args.input = argv[++i]; }
        else if (a == "--output-dir" && i + 1 < argc) { args.outputDir = argv[++i]; }
        else if (a == "--no-blind")                    { args.noBlind = true; }
        else if (a == "--cutflow-only")                { args.cutflowOnly = true; }
        else if (a == "--schemes") {
            while (i + 1 < argc && argv[i + 1][0] != '-') {
                args.schemes.push_back(argv[++i]);
            }
        } else {
            std::cerr << "Unknown argument: " << a << "\n"
                      << "Usage: run_analysis [--input FILE] [--output-dir DIR] "
                         "[--schemes s1 s2 ...] [--no-blind] [--cutflow-only]\n";
            std::exit(1);
        }
    }
    return args;
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
    CLIArgs args = parseArgs(argc, argv);

    // Determine which schemes to run
    const auto& allSchemes = getSchemes();
    std::vector<std::string> schemeKeys;
    if (args.schemes.empty()) {
        for (auto& [key, _] : allSchemes) schemeKeys.push_back(key);
    } else {
        for (auto& s : args.schemes) {
            if (allSchemes.find(s) == allSchemes.end()) {
                std::cerr << "ERROR: Unknown scheme '" << s << "'. Available:";
                for (auto& [k, _] : allSchemes) std::cerr << " " << k;
                std::cerr << std::endl;
                return 1;
            }
            schemeKeys.push_back(s);
        }
    }

    std::cout << "=== HH->bbgg Analysis ===" << std::endl;
    std::cout << "Input:   " << args.input << std::endl;
    std::cout << "Output:  " << args.outputDir << std::endl;
    std::cout << "Schemes:";
    for (auto& s : schemeKeys) std::cout << " " << s;
    std::cout << std::endl;
    std::cout << "Blind:   " << (args.noBlind ? "OFF" : "ON") << std::endl;

    // Open data
    DataLoader loader(args.input);
    EventData evt;
    loader.setupBranches(evt);

    // Setup scheme data — one SchemeData per scheme, all connected to the same TTree
    std::map<std::string, SchemeData> schemeDatas;
    for (auto& key : schemeKeys) {
        schemeDatas[key] = SchemeData{};
        loader.setupSchemeBranches(schemeDatas[key], key);
    }

    // Selection
    EventSelector selector;

    // ----- Cutflow-only mode -----
    if (args.cutflowOnly) {
        for (auto& key : schemeKeys) {
            selector.printCutflow(loader, key);
        }
        return 0;
    }

    // ----- Book histograms -----
    Plotter plotter(args.outputDir);
    auto commonDefs = getPlotDefs();
    auto schemeDefs = getSchemePlotDefs();

    // Common histograms
    std::map<std::string, TH1D*> hCommon;
    for (auto& [varName, def] : commonDefs) {
        hCommon[varName] = plotter.bookTH1(varName, def);
    }

    // Per-scheme histograms
    std::map<std::string, std::map<std::string, TH1D*>> hScheme;
    std::map<std::string, TH2D*> h2D_massPlane;
    for (auto& key : schemeKeys) {
        for (auto& [varName, def] : schemeDefs) {
            std::string hname = key + "_" + varName;
            hScheme[key][varName] = plotter.bookTH1(hname, def);
        }
        // 2D: mgg vs mjj
        std::string h2name = key + "_mgg_vs_mjj";
        h2D_massPlane[key] = plotter.bookTH2(h2name, 40, 100, 180, 40, 0, 300,
                                              "m_{#gamma#gamma} [GeV]", "m_{jj} [GeV]");
    }

    // ----- Event loop -----
    Long64_t nEntries = loader.getEntries();
    std::cout << "\nProcessing " << nEntries << " events..." << std::endl;

    bool doBlind = !args.noBlind;

    for (Long64_t i = 0; i < nEntries; ++i) {
        loader.getEntry(i);

        double w = evt.weight;

        // Fill common histograms (no scheme requirement)
        bool blindVeto = doBlind && (evt.mass >= BLIND_LOW && evt.mass <= BLIND_HIGH);

        if (!blindVeto) hCommon["mass"]->Fill(evt.mass, w);
        hCommon["pt"]->Fill(evt.pt, w);
        hCommon["eta"]->Fill(evt.eta, w);
        hCommon["phi"]->Fill(evt.phi, w);

        hCommon["lead_pt"]->Fill(evt.lead_pt, w);
        hCommon["lead_eta"]->Fill(evt.lead_eta, w);
        hCommon["lead_mvaID"]->Fill(evt.lead_mvaID, w);
        hCommon["lead_r9"]->Fill(evt.lead_r9, w);

        hCommon["sublead_pt"]->Fill(evt.sublead_pt, w);
        hCommon["sublead_eta"]->Fill(evt.sublead_eta, w);
        hCommon["sublead_mvaID"]->Fill(evt.sublead_mvaID, w);
        hCommon["sublead_r9"]->Fill(evt.sublead_r9, w);

        hCommon["MultiBDT_output_0"]->Fill(evt.MultiBDT_output[0], w);
        hCommon["MultiBDT_output_1"]->Fill(evt.MultiBDT_output[1], w);
        hCommon["MultiBDT_output_2"]->Fill(evt.MultiBDT_output[2], w);
        hCommon["MultiBDT_output_3"]->Fill(evt.MultiBDT_output[3], w);

        hCommon["n_jets"]->Fill(evt.n_jets, w);
        hCommon["nBLoose"]->Fill(evt.nBLoose, w);
        hCommon["nBMedium"]->Fill(evt.nBMedium, w);
        hCommon["nBTight"]->Fill(evt.nBTight, w);

        hCommon["puppiMET_pt"]->Fill(evt.puppiMET_pt, w);
        hCommon["puppiMET_phi"]->Fill(evt.puppiMET_phi, w);

        hCommon["sigma_m_over_m"]->Fill(evt.sigma_m_over_m, w);

        hCommon["alpha"]->Fill(evt.alpha, w);
        hCommon["beta"]->Fill(evt.beta, w);
        hCommon["gamma"]->Fill(evt.gamma, w);
        hCommon["D_ttH"]->Fill(evt.D_ttH, w);
        hCommon["D_qcd"]->Fill(evt.D_qcd, w);

        // Per-scheme histograms
        for (auto& key : schemeKeys) {
            SchemeData& sd = schemeDatas[key];

            if (!selector.passSchemeFlag(evt, key)) continue;
            if (!selector.passPreselection(evt, sd, key)) continue;

            auto& hs = hScheme[key];
            hs["dijet_mass"]->Fill(sd.dijet_mass, w);
            hs["dijet_mass_DNNreg"]->Fill(sd.dijet_mass_DNNreg, w);
            hs["dijet_pt"]->Fill(sd.dijet_pt, w);

            hs["lead_bjet_pt"]->Fill(sd.lead_bjet_pt, w);
            hs["lead_bjet_eta"]->Fill(sd.lead_bjet_eta, w);
            hs["lead_bjet_btagPNetB"]->Fill(sd.lead_bjet_btagPNetB, w);
            hs["lead_bjet_btagUParTAK4B"]->Fill(sd.lead_bjet_btagUParTAK4B, w);

            hs["sublead_bjet_pt"]->Fill(sd.sublead_bjet_pt, w);
            hs["sublead_bjet_eta"]->Fill(sd.sublead_bjet_eta, w);
            hs["sublead_bjet_btagPNetB"]->Fill(sd.sublead_bjet_btagPNetB, w);
            hs["sublead_bjet_btagUParTAK4B"]->Fill(sd.sublead_bjet_btagUParTAK4B, w);

            hs["HHbbggCandidate_mass"]->Fill(sd.HHbbggCandidate_mass, w);
            hs["HHbbggCandidate_pt"]->Fill(sd.HHbbggCandidate_pt, w);

            hs["CosThetaStar_CS"]->Fill(sd.CosThetaStar_CS, w);
            hs["DeltaR_jg_min"]->Fill(sd.DeltaR_jg_min, w);
            hs["M_X"]->Fill(sd.M_X, w);
            hs["chi_t0"]->Fill(sd.chi_t0, w);
            hs["chi_t1"]->Fill(sd.chi_t1, w);
            hs["pholead_PtOverM"]->Fill(sd.pholead_PtOverM, w);
            hs["phosublead_PtOverM"]->Fill(sd.phosublead_PtOverM, w);

            // 2D mass plane (apply blinding on mgg axis)
            if (!blindVeto) {
                h2D_massPlane[key]->Fill(evt.mass, sd.dijet_mass, w);
            }
        }
    }
    std::cout << "Event loop complete." << std::endl;

    // ----- Draw & save common histograms -----
    std::cout << "Drawing common histograms..." << std::endl;
    for (auto& [varName, h] : hCommon) {
        if (varName == "mass" && doBlind) {
            plotter.draw1D(h, BLIND_LOW, BLIND_HIGH);
        } else {
            plotter.draw1D(h);
        }
    }

    // ----- Draw & save per-scheme histograms -----
    for (auto& key : schemeKeys) {
        std::cout << "Drawing histograms for scheme: " << key << std::endl;
        for (auto& [varName, h] : hScheme[key]) {
            plotter.draw1D(h);
        }
        plotter.draw2DMassPlane(h2D_massPlane[key], doBlind);
    }

    // ----- Cross-scheme comparison plots -----
    if (schemeKeys.size() > 1) {
        std::cout << "Drawing cross-scheme comparisons..." << std::endl;
        std::vector<std::string> compareVars = {
            "dijet_mass", "dijet_mass_DNNreg", "HHbbggCandidate_mass",
            "lead_bjet_pt", "sublead_bjet_pt", "CosThetaStar_CS"
        };
        for (auto& varName : compareVars) {
            std::vector<TH1D*> hists;
            std::vector<std::string> labels;
            for (auto& key : schemeKeys) {
                if (hScheme[key].count(varName)) {
                    hists.push_back(hScheme[key][varName]);
                    labels.push_back(allSchemes.at(key).name);
                }
            }
            if (hists.size() > 1) {
                plotter.drawCompare(hists, labels, true);
            }
        }
    }

    // ----- Cutflow tables -----
    std::cout << "\n--- Cutflow Tables ---" << std::endl;
    for (auto& key : schemeKeys) {
        selector.printCutflow(loader, key);
    }

    std::cout << "\nDone! Plots saved to " << args.outputDir << "/" << std::endl;
    return 0;
}
