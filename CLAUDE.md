# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Search for nonresonant Higgs boson pair (HH) production in the bbγγ final channel for the **CMS experiment** at the LHC. The goal is to improve upon the existing public result [CMS-PAS-HIG-25-007](https://cms-results.web.cern.ch/cms-results/public-results/preliminary-results/HIG-25-007/index.html).

### Existing Analysis Baseline (HIG-25-007)
- **Dataset**: pp collisions at √s = 13.6 TeV, 61.9 fb⁻¹ (2022–2023, Run 3)
- **Signal extraction**: 2D simultaneous fit to mγγ and mjj, or 1D fit to mγγ alone
- **Event categorization**: Three categories from a multivariate discriminator (decreasing S/B) plus a boosted category
- **Background**: Nonresonant backgrounds modeled from sideband fits to mγγ (blinded region 115–135 GeV); discrete profiling for shape systematics
- **Current best limits**: 7.3× SM expected (2D), observed 11.0× SM; κλ constraints from −5 to 12

### Goals for This Repository
- Improve signal (HH) vs SM background separation using **ML techniques**
- Perform **mass fits** (mγγ, mjj) for signal extraction
- Tighten limits on HH production cross section and Higgs self-coupling κλ

## Languages and Tools

- **ROOT** (C++ and PyROOT): data I/O via TTree/RDataFrame, histogramming, fitting (RooFit)
- **Python**: analysis scripting, ML training and evaluation, plotting
- **ML frameworks**: e.g. XGBoost, scikit-learn, or PyTorch for multivariate classifiers

## Key Physics Concepts

- **Signal**: HH→bbγγ — one Higgs decays to two b-quarks, the other to two photons
- **Backgrounds**: single Higgs (ggH, ttH, VH, VBF with H→γγ), continuum γγ+jets, tt̄+γγ
- **Discriminating variables**: mγγ, mjj, MVA score, pT of the HH system, b-tagging scores, photon ID
- **κλ (kappa-lambda)**: Higgs trilinear self-coupling modifier — the primary physics parameter of interest

## Project Status

Early-stage repository — initial commit. No source code, build system, or tests yet.
