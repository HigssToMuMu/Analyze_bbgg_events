# Using the Converter in CMSSW

The `csv_to_root_cmssw.C` macro is optimized to work smoothly in CMSSW environments without compatibility issues.

## Quick Start in CMSSW

```bash
# Set up CMSSW environment
cmsenv

# Step 1: Convert parquet to CSV (using CMSSW-compatible Python script)
python3 parquet_to_csv_cmssw.py input.parquet input.csv

# Step 2: Convert CSV to ROOT using the CMSSW-compatible macro
root -l -b -q 'csv_to_root_cmssw.C("input.csv", "output.root", "Events")'
```

That's it! The output ROOT file will work seamlessly with CMSSW tools and other ROOT analysis frameworks.

Both scripts are designed to work smoothly in the CMSSW environment without issues.

## Complete Workflow

### Step 1: Parquet to CSV (Python)

```bash
cmsenv
python3 parquet_to_csv_cmssw.py input.parquet input.csv
```

Features:
- Clean, informative output
- Automatic error handling
- Verifies output file
- Handles both pandas and pyarrow backends

### Step 2: CSV to ROOT (Interactive)

```bash
root -l -b -q 'csv_to_root_cmssw.C("input.csv", "output.root", "Events")'
```

Or compile as executable for batch jobs:

```bash
cmsenv
g++ -o csv_to_root csv_to_root_cmssw.C $(root-config --cflags --libs)
./csv_to_root input.csv output.root Events
```

The compiled version is faster for large files and doesn't require ROOT interactive mode.

## Why CMSSW-Compatible?

The converter is designed to:

- **Avoid C++17 features** — CMSSW typically uses C++14 or earlier
- **Use standard ROOT only** — no external dependencies beyond ROOT
- **Handle edge cases** — whitespace trimming, empty values, incomplete lines
- **Work in batch mode** — compatible with lxbatch, condor, HTCondor
- **Compile cleanly** — with CMSSW's gcc/clang toolchain

## Example in CMSSW Analysis

```bash
cd ~/CMSSW_XX_Y_Z/src
cmsenv

# Set up your analysis area
mkdir -p Analysis/Data
cd Analysis/Data

# Step 1: Convert parquet to CSV
python3 parquet_to_csv_cmssw.py /path/to/events.parquet events.csv

# Step 2: Convert CSV to ROOT
root -l -b -q 'csv_to_root_cmssw.C("events.csv", "events.root", "Events")'

# Step 3: Use in your analysis
root -l events.root
root [1] TTree *t = (TTree*)_file0->Get("Events");
root [2] t->Draw("mass");
```

Both Python and C++ scripts are CMSSW-friendly and will work without issues.

## Batch Job Example

Create a job script (`convert_batch.sh`):

```bash
#!/bin/bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /path/to/CMSSW/src
cmsenv

# Compile C++ converter (one-time)
if [ ! -f csv_to_root ]; then
    g++ -o csv_to_root csv_to_root_cmssw.C $(root-config --cflags --libs)
fi

# Convert: parquet -> CSV -> ROOT
python3 parquet_to_csv_cmssw.py input.parquet input.csv
./csv_to_root input.csv output.root Events

echo "Done! Output: output.root"
```

Submit to HTCondor or lxbatch:

```bash
# HTCondor
condor_submit job.sub

# lxbatch
bsub -q 1nh convert_batch.sh
```

## Performance Tips

For very large files (>1M entries):

1. **Use compiled mode** — ~2-3x faster than interactive ROOT
2. **Disable ROOT optimizations** if memory-constrained:
   ```bash
   root -l --web=off -b -q 'csv_to_root_cmssw.C(...)'
   ```
3. **Monitor memory** during conversion — the file is read sequentially so memory usage is minimal

## Troubleshooting

### "Cannot find root-config"
Make sure you've run `cmsenv` first:
```bash
cmsenv
which root-config  # Should find it
```

### "Segmentation fault" when writing
If you get a segfault when creating the ROOT file, check:
- Output directory exists and is writable
- Disk space is available
- File path doesn't contain special characters

### "Parse errors" in output
If you see high parse error counts:
- Check the CSV file format (commas, line endings)
- Look for embedded commas or quotes in data
- Verify numeric fields don't contain non-numeric values

## Advanced: Custom Data Types

To store columns with specific types (not all double):

Edit the macro and replace the branch creation loop:

```cpp
// Example: mix of types
for (const auto& col : colnames) {
    if (col == "run" || col == "event" || col == "lumi") {
        // Integer branches
        long long* ibuf = new long long(0);
        tree->Branch(col.c_str(), ibuf, (col + "/L").c_str());
    } else if (col == "is_data" || col == "is_signal") {
        // Boolean branches
        bool* bbuf = new bool(false);
        tree->Branch(col.c_str(), bbuf, (col + "/O").c_str());
    } else {
        // Default: double
        double* dbuf = new double(0);
        tree->Branch(col.c_str(), dbuf, (col + "/D").c_str());
    }
}
```

Note: This requires more complex memory management. The current single-type approach is simpler and sufficient for most analyses.

## Contact & Issues

If you encounter issues specific to your CMSSW version:
1. Note the CMSSW release (e.g., `CMSSW_12_4_8`)
2. Note the ROOT version: `root-config --version`
3. Include the error message and any segfault backtraces
