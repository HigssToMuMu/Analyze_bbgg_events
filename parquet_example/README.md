# Parquet to ROOT Converter

Convert parquet data files to ROOT ntuples (flat TTree format) for use with native ROOT.

## Quick Start

### Option 1: Convert an existing parquet file (with Python)

```bash
cd parquet_example

# Step 1: Convert parquet to CSV
python3 -c "
import pandas as pd
df = pd.read_parquet('your_file.parquet')
df.to_csv('your_file.csv', index=False)
print(f'Wrote your_file.csv')
"

# Step 2: Convert CSV to ROOT (pure ROOT, no Python needed)
root -l -b -q 'parquet_to_root_generic.C("your_file.csv", "your_file.root", "Events")'
```

### Option 2: Use the Python helper script (if you have pandas installed)

```bash
cd parquet_example
python3 convert_parquet.py  # Converts events.parquet -> events.csv
root -l -b -q 'parquet_to_root_generic.C("events.csv", "events.root", "Events")'
```

## How to Write Your Own Converter

The ROOT macro approach is the key — it's native, dependency-free, and portable.

### 1. Convert parquet to CSV (one-time setup)

Use any tool that reads parquet and writes CSV. Example with pandas:

```python
import pandas as pd

df = pd.read_parquet('input.parquet')
df.to_csv('input.csv', index=False)
```

Or use DuckDB (installed on most systems):

```bash
duckdb -c "COPY (SELECT * FROM 'input.parquet') TO 'input.csv' (HEADER true);"
```

### 2. Create a ROOT macro to convert CSV to ROOT

Create a file named `csv_to_root.C`:

```cpp
// CSV to ROOT TTree converter
// Usage: root -l -b -q 'csv_to_root.C("input.csv", "output.root", "TreeName")'

void csv_to_root(const char* csvfile, const char* rootfile, const char* treename = "data") {
    std::cout << "Converting " << csvfile << " to " << rootfile << std::endl;

    TFile* f = new TFile(rootfile, "RECREATE");
    TTree* tree = new TTree(treename, treename);

    // Read header
    std::ifstream file(csvfile);
    std::string header;
    std::getline(file, header);

    // Parse column names
    std::vector<std::string> colnames;
    std::istringstream iss(header);
    std::string col;
    while (std::getline(iss, col, ',')) {
        colnames.push_back(col);
    }

    std::cout << "Columns: " << colnames.size() << std::endl;

    // Create branches (all as double)
    std::map<std::string, double> buf;
    for (const auto& col : colnames) {
        buf[col] = 0;
        tree->Branch(col.c_str(), &buf[col], (col + "/D").c_str());
    }

    // Read data
    std::string line;
    int nlines = 0;
    while (std::getline(file, line)) {
        std::istringstream linestream(line);
        std::string val;
        size_t col_idx = 0;

        while (std::getline(linestream, val, ',') && col_idx < colnames.size()) {
            try {
                buf[colnames[col_idx]] = std::stod(val);
            } catch (...) {
                buf[colnames[col_idx]] = 0;
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
    std::cout << "Written " << nlines << " entries to " << rootfile << std::endl;

    tree->Write();
    f->Close();
}
```

### 3. Run the converter

```bash
root -l -b -q 'csv_to_root.C("input.csv", "output.root", "MyTreeName")'
```

### 4. Verify the output

```bash
root -l output.root
root [1] TTree *t = (TTree*)_file0->Get("MyTreeName");
root [2] t->Print();
root [3] t->Scan("col1:col2:col3", "", "", 5);  // Print first 5 entries
```

## Committing and Pushing to Git

Once you have created your converter and tested it:

```bash
# Add the converter macro to git
git add path/to/csv_to_root.C

# Commit with a clear message
git commit -m "Add CSV-to-ROOT converter macro"

# Push to remote
git push origin main
```

Example with complete workflow:

```bash
# Create converter
cat > my_converter.C << 'EOF'
void csv_to_root(const char* csvfile, const char* rootfile, const char* treename = "data") {
    // ... macro code ...
}
EOF

# Test it
root -l -b -q 'my_converter.C("test.csv", "test.root", "Events")'

# If test passes, commit
git add my_converter.C
git commit -m "Add generic CSV-to-ROOT converter"
git push origin main
```

## Why This Approach Works

1. **No uproot compatibility issues** — uproot-created files sometimes segfault in ROOT C++
2. **No external dependencies** — only ROOT itself, which you already have
3. **Fast** — reads/writes millions of rows in seconds
4. **Portable** — works on any system with ROOT
5. **Easy to customize** — simple C++ macro, easy to modify for specific needs

## Limitations

- All columns are stored as `double` (64-bit float)
- CSV parsing assumes simple comma-separated values (no embedded commas/newlines)
- Header row is required

## Customization Examples

### Store columns as different types

Modify the macro to use `float`, `int`, `bool`, etc:

```cpp
// For integer columns:
int intval = 0;
tree->Branch("count", &intval, "count/I");  // I = int32

// For float columns:
float fval = 0;
tree->Branch("efficiency", &fval, "efficiency/F");  // F = float32

// For bool columns:
bool bval = false;
tree->Branch("flag", &bval, "flag/O");  // O = bool
```

### Add cuts/filtering

Skip rows during conversion:

```cpp
if (/* some condition */) continue;  // Skip this row
```

### Handle missing values

Replace NaN with a sentinel:

```cpp
if (std::isnan(buf[col])) {
    buf[col] = -999;  // Your sentinel value
}
```
