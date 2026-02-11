#ifndef PLOTTER_H
#define PLOTTER_H

#include "Config.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TStyle.h>

class Plotter {
public:
    Plotter(const std::string& outputDir, double lumi = LUMI_RUN3, double sqrtS = SQRT_S);
    ~Plotter();

    // Histogram booking
    TH1D* bookTH1(const std::string& name, const PlotDef& def);
    TH2D* bookTH2(const std::string& name, int nx, double xmin, double xmax,
                   int ny, double ymin, double ymax,
                   const std::string& xlabel = "", const std::string& ylabel = "");

    // Drawing
    void draw1D(TH1D* h, double blindLow = -1, double blindHigh = -1);
    void drawCompare(const std::vector<TH1D*>& hists, const std::vector<std::string>& labels,
                     bool normalize = true);
    void draw2DMassPlane(TH2D* h, bool blind = true);
    void drawCutflow(const std::map<std::string, int>& cuts, const std::string& schemeName);

    // Saving
    void save(TCanvas* c, const std::string& name);

    // CMS style helpers
    static void setupCMSStyle();
    void drawCMSLabel(TCanvas* c, const std::string& extra = "Preliminary");

private:
    std::string outputDir_;
    double lumi_;
    double sqrtS_;
    std::vector<std::unique_ptr<TH1D>> ownedTH1_;
    std::vector<std::unique_ptr<TH2D>> ownedTH2_;
};

#endif
