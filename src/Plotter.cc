#include "Plotter.h"
#include "Utils.h"
#include <TLatex.h>
#include <TLegend.h>
#include <TBox.h>
#include <TLine.h>
#include <TROOT.h>
#include <iostream>
#include <sstream>

// Color palette for overlays
static const int kSchemeColors[] = {kBlue+1, kRed+1, kGreen+2, kMagenta+1, kOrange+1, kCyan+2};
static const int nSchemeColors = 6;

Plotter::Plotter(const std::string& outputDir, double lumi, double sqrtS)
    : outputDir_(outputDir), lumi_(lumi), sqrtS_(sqrtS) {
    ensureDirectory(outputDir_);
    setupCMSStyle();
}

Plotter::~Plotter() = default;

void Plotter::setupCMSStyle() {
    auto* style = new TStyle("CMS", "CMS Style");
    style->SetOptStat(0);
    style->SetOptTitle(0);
    style->SetPadTickX(1);
    style->SetPadTickY(1);
    style->SetPadTopMargin(0.06);
    style->SetPadBottomMargin(0.13);
    style->SetPadLeftMargin(0.16);
    style->SetPadRightMargin(0.04);
    style->SetTitleFont(42, "XYZ");
    style->SetTitleSize(0.05, "XYZ");
    style->SetTitleOffset(1.1, "X");
    style->SetTitleOffset(1.4, "Y");
    style->SetLabelFont(42, "XYZ");
    style->SetLabelSize(0.04, "XYZ");
    style->SetFrameLineWidth(2);
    style->SetHistLineWidth(2);
    style->SetEndErrorSize(0);
    gROOT->SetStyle("CMS");
    gROOT->ForceStyle();
}

TH1D* Plotter::bookTH1(const std::string& name, const PlotDef& def) {
    std::string title = ";" + def.xlabel;
    if (!def.units.empty()) title += " [" + def.units + "]";
    title += ";Events";
    if (def.xmax > def.xmin && def.nbins > 0) {
        double bw = (def.xmax - def.xmin) / def.nbins;
        std::ostringstream oss;
        oss.precision(1);
        oss << std::fixed << bw;
        title += " / " + oss.str();
        if (!def.units.empty()) title += " " + def.units;
    }

    auto h = std::make_unique<TH1D>(name.c_str(), title.c_str(), def.nbins, def.xmin, def.xmax);
    h->SetLineColor(kBlack);
    h->SetLineWidth(2);
    h->Sumw2();
    TH1D* ptr = h.get();
    ownedTH1_.push_back(std::move(h));
    return ptr;
}

TH2D* Plotter::bookTH2(const std::string& name, int nx, double xmin, double xmax,
                        int ny, double ymin, double ymax,
                        const std::string& xlabel, const std::string& ylabel) {
    std::string title = ";" + xlabel + ";" + ylabel;
    auto h = std::make_unique<TH2D>(name.c_str(), title.c_str(), nx, xmin, xmax, ny, ymin, ymax);
    TH2D* ptr = h.get();
    ownedTH2_.push_back(std::move(h));
    return ptr;
}

void Plotter::drawCMSLabel(TCanvas* c, const std::string& extra) {
    c->cd();
    TLatex latex;
    latex.SetNDC();

    // "CMS" bold
    latex.SetTextFont(61);
    latex.SetTextSize(0.05);
    latex.DrawLatex(0.16, 0.95, "CMS");

    // Extra text (e.g. "Preliminary")
    if (!extra.empty()) {
        latex.SetTextFont(52);
        latex.SetTextSize(0.04);
        latex.DrawLatex(0.27, 0.95, extra.c_str());
    }

    // Lumi + sqrt(s)
    std::ostringstream oss;
    oss.precision(1);
    oss << std::fixed << lumi_ << " fb^{-1} (" << sqrtS_ << " TeV)";
    latex.SetTextFont(42);
    latex.SetTextSize(0.04);
    latex.SetTextAlign(31);
    latex.DrawLatex(0.96, 0.95, oss.str().c_str());
}

void Plotter::draw1D(TH1D* h, double blindLow, double blindHigh) {
    TCanvas c("c", "", 800, 600);
    h->Draw("HIST E");
    drawCMSLabel(&c);

    // Blinding band
    if (blindLow > 0 && blindHigh > 0 && blindLow < blindHigh) {
        double ymax = h->GetMaximum() * 1.15;
        TBox box(blindLow, 0, blindHigh, ymax);
        box.SetFillColor(kGray);
        box.SetFillStyle(3354);
        box.Draw("same");
        h->Draw("HIST E same"); // redraw on top
    }

    save(&c, h->GetName());
}

void Plotter::drawCompare(const std::vector<TH1D*>& hists, const std::vector<std::string>& labels,
                           bool normalize) {
    if (hists.empty()) return;

    TCanvas c("c_compare", "", 800, 600);

    TLegend leg(0.60, 0.70, 0.92, 0.92);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.SetTextFont(42);
    leg.SetTextSize(0.035);

    double globalMax = 0;
    for (size_t i = 0; i < hists.size(); ++i) {
        TH1D* h = hists[i];
        h->SetLineColor(kSchemeColors[i % nSchemeColors]);
        h->SetLineWidth(2);
        h->SetFillStyle(0);
        if (normalize && h->Integral() > 0) {
            h->Scale(1.0 / h->Integral());
        }
        if (h->GetMaximum() > globalMax) globalMax = h->GetMaximum();
        leg.AddEntry(h, labels[i].c_str(), "l");
    }

    for (size_t i = 0; i < hists.size(); ++i) {
        hists[i]->SetMaximum(globalMax * 1.3);
        hists[i]->Draw(i == 0 ? "HIST E" : "HIST E same");
    }
    leg.Draw();
    drawCMSLabel(&c);

    std::string saveName = std::string("compare_") + hists[0]->GetName();
    save(&c, saveName);
}

void Plotter::draw2DMassPlane(TH2D* h, bool blind) {
    TCanvas c("c_2d", "", 800, 700);
    c.SetRightMargin(0.15);
    h->Draw("COLZ");

    // Draw crosshair lines at m_H = 125 GeV
    TLine lineX(h->GetXaxis()->GetXmin(), HIGGS_MASS, h->GetXaxis()->GetXmax(), HIGGS_MASS);
    lineX.SetLineStyle(2);
    lineX.SetLineColor(kRed);
    lineX.SetLineWidth(2);
    lineX.Draw("same");

    TLine lineY(HIGGS_MASS, h->GetYaxis()->GetXmin(), HIGGS_MASS, h->GetYaxis()->GetXmax());
    lineY.SetLineStyle(2);
    lineY.SetLineColor(kRed);
    lineY.SetLineWidth(2);
    lineY.Draw("same");

    if (blind) {
        TBox box(BLIND_LOW, h->GetYaxis()->GetXmin(), BLIND_HIGH, h->GetYaxis()->GetXmax());
        box.SetFillColor(kGray);
        box.SetFillStyle(3354);
        box.Draw("same");
    }

    drawCMSLabel(&c);
    save(&c, h->GetName());
}

void Plotter::drawCutflow(const std::map<std::string, int>& cuts, const std::string& schemeName) {
    int nCuts = static_cast<int>(cuts.size());
    if (nCuts == 0) return;

    TH1D hCut(("cutflow_" + schemeName).c_str(), ";Cut;Events", nCuts, 0, nCuts);
    hCut.SetFillColor(kAzure + 1);
    hCut.SetLineColor(kAzure + 2);

    int bin = 1;
    for (auto& [label, count] : cuts) {
        hCut.GetXaxis()->SetBinLabel(bin, label.c_str());
        hCut.SetBinContent(bin, count);
        bin++;
    }
    hCut.GetXaxis()->SetLabelSize(0.035);
    hCut.LabelsOption("v");

    TCanvas c("c_cutflow", "", 900, 600);
    c.SetBottomMargin(0.25);
    hCut.Draw("BAR");
    drawCMSLabel(&c, "Preliminary");
    save(&c, "cutflow_" + schemeName);
}

void Plotter::save(TCanvas* c, const std::string& name) {
    std::string base = outputDir_ + "/" + name;
    c->SaveAs((base + ".pdf").c_str());
    c->SaveAs((base + ".png").c_str());
}
