// CSV to ROOT TTree converter optimized for CMSSW environment
// Works with ROOT as built in CMSSW (usually older versions like 6.22, 6.26)
//
// Usage in CMSSW:
//   cmsenv
//   root -l -b -q 'csv_to_root_cmssw.C("input.csv", "output.root", "Events")'
//
// Or compile as standalone executable:
//   cmsenv
//   g++ -o csv_to_root csv_to_root_cmssw.C $(root-config --cflags --libs)
//   ./csv_to_root input.csv output.root Events

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

// Standalone function that can be called from ROOT or compiled
void csv_to_root_cmssw(const char* csvfile, const char* rootfile, const char* treename = "data") {
    std::cout << "=== CSV to ROOT Converter (CMSSW compatible) ===" << std::endl;
    std::cout << "Input:  " << csvfile << std::endl;
    std::cout << "Output: " << rootfile << std::endl;
    std::cout << "Tree:   " << treename << std::endl << std::endl;

    // Open input file
    std::ifstream infile(csvfile);
    if (!infile.is_open()) {
        std::cerr << "ERROR: Cannot open input file: " << csvfile << std::endl;
        return;
    }

    // Read header line
    std::string header;
    if (!std::getline(infile, header)) {
        std::cerr << "ERROR: Cannot read header from CSV file" << std::endl;
        return;
    }

    // Parse column names from header
    std::vector<std::string> colnames;
    std::istringstream header_stream(header);
    std::string colname;
    while (std::getline(header_stream, colname, ',')) {
        // Trim whitespace
        size_t start = colname.find_first_not_of(" \t\r\n");
        size_t end = colname.find_last_not_of(" \t\r\n");
        if (start != std::string::npos) {
            colname = colname.substr(start, end - start + 1);
        }
        colnames.push_back(colname);
    }

    std::cout << "Found " << colnames.size() << " columns:" << std::endl;
    for (size_t i = 0; i < colnames.size() && i < 10; i++) {
        std::cout << "  [" << i << "] " << colnames[i] << std::endl;
    }
    if (colnames.size() > 10) {
        std::cout << "  ... and " << (colnames.size() - 10) << " more" << std::endl;
    }
    std::cout << std::endl;

    // Create ROOT file and tree
    TFile* outfile = new TFile(rootfile, "RECREATE");
    if (outfile->IsZombie()) {
        std::cerr << "ERROR: Cannot create ROOT file: " << rootfile << std::endl;
        return;
    }

    TTree* tree = new TTree(treename, treename);

    // Create buffer and branches for each column (all as double)
    std::map<std::string, double> branch_buffers;
    for (const auto& col : colnames) {
        branch_buffers[col] = 0.0;
        TBranch* br = tree->Branch(col.c_str(), &branch_buffers[col], (col + "/D").c_str());
        if (!br) {
            std::cerr << "WARNING: Failed to create branch: " << col << std::endl;
        }
    }

    // Read data lines and fill tree
    std::string line;
    long long nlines = 0;
    long long nerrors = 0;

    std::cout << "Processing data..." << std::endl;

    while (std::getline(infile, line)) {
        std::istringstream line_stream(line);
        std::string value_str;
        size_t col_idx = 0;

        // Parse each column value
        while (std::getline(line_stream, value_str, ',') && col_idx < colnames.size()) {
            // Trim whitespace
            size_t start = value_str.find_first_not_of(" \t\r\n");
            size_t end = value_str.find_last_not_of(" \t\r\n");
            if (start != std::string::npos) {
                value_str = value_str.substr(start, end - start + 1);
            }

            // Convert to double
            try {
                if (value_str.empty()) {
                    branch_buffers[colnames[col_idx]] = 0.0;
                } else {
                    branch_buffers[colnames[col_idx]] = std::stod(value_str);
                }
            } catch (const std::exception& e) {
                branch_buffers[colnames[col_idx]] = 0.0;
                nerrors++;
            }

            col_idx++;
        }

        // Fill remaining columns with zeros if line is incomplete
        while (col_idx < colnames.size()) {
            branch_buffers[colnames[col_idx]] = 0.0;
            col_idx++;
        }

        tree->Fill();

        nlines++;
        if (nlines % 50000 == 0) {
            std::cout << "  " << nlines << " entries processed" << std::endl;
        }
    }

    infile.close();

    std::cout << std::endl;
    std::cout << "=== Conversion Complete ===" << std::endl;
    std::cout << "Entries written:  " << nlines << std::endl;
    std::cout << "Parse errors:     " << nerrors << std::endl;
    std::cout << "Branches:         " << tree->GetNbranches() << std::endl;

    // Write to file and close
    tree->Write();
    outfile->Close();

    std::cout << "Output file:      " << rootfile << std::endl;
    std::cout << "Done!" << std::endl;
}

// If compiled as standalone program
#ifndef __CLING__
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: csv_to_root <input.csv> <output.root> [treename]" << std::endl;
        return 1;
    }

    const char* treename = (argc > 3) ? argv[3] : "data";
    csv_to_root_cmssw(argv[1], argv[2], treename);

    return 0;
}
#endif
