// Generic CSV to ROOT TTree converter
// Converts any flat CSV file (single header row) to a ROOT TTree
// All columns are stored as doubles
//
// Usage:
//   root -l -b -q 'parquet_to_root_generic.C("input.csv", "output.root", "TreeName")'
//
// Example:
//   root -l -b -q 'parquet_to_root_generic.C("events.csv", "events.root", "Events")'

void parquet_to_root_generic(const char* csvfile, const char* rootfile, const char* treename = "data") {
    std::cout << "Converting " << csvfile << " to " << rootfile << std::endl;

    TFile* f = new TFile(rootfile, "RECREATE");
    TTree* tree = new TTree(treename, treename);

    // Read CSV header to get column names
    std::ifstream file(csvfile);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open " << csvfile << std::endl;
        return;
    }

    std::string header;
    std::getline(file, header);

    // Parse header
    std::vector<std::string> colnames;
    std::istringstream iss(header);
    std::string col;
    while (std::getline(iss, col, ',')) {
        colnames.push_back(col);
    }

    std::cout << "Columns: " << colnames.size() << std::endl;
    for (size_t i = 0; i < colnames.size(); i++) {
        if (i < 10) std::cout << "  " << i << ": " << colnames[i] << std::endl;
    }
    if (colnames.size() > 10) std::cout << "  ... and " << (colnames.size() - 10) << " more" << std::endl;

    // Create branches and buffers (all as double)
    std::map<std::string, double> buf;
    for (const auto& col : colnames) {
        buf[col] = 0;
        tree->Branch(col.c_str(), &buf[col], (col + "/D").c_str());
    }

    // Read data lines
    std::string line;
    int nlines = 0;
    int errors = 0;
    while (std::getline(file, line)) {
        std::istringstream linestream(line);
        std::string val;
        size_t col_idx = 0;

        while (std::getline(linestream, val, ',') && col_idx < colnames.size()) {
            try {
                buf[colnames[col_idx]] = std::stod(val);
            } catch (...) {
                buf[colnames[col_idx]] = 0;
                errors++;
            }
            col_idx++;
        }

        tree->Fill();

        nlines++;
        if (nlines % 50000 == 0) {
            std::cout << nlines << " entries processed" << std::endl;
        }
    }

    file.close();

    std::cout << "\nWritten " << nlines << " entries to " << rootfile << std::endl;
    if (errors > 0) std::cout << "  (with " << errors << " parse errors)" << std::endl;
    std::cout << "File size: " << (f->GetSize() / 1024.0 / 1024.0) << " MB" << std::endl;

    tree->Write();
    f->Close();

    std::cout << "Done!" << std::endl;
}
