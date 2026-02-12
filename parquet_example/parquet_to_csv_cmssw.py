#!/usr/bin/env python3
"""
Parquet to CSV converter optimized for CMSSW environments.

Works with Python 3.6+ as available in CMSSW.
Minimal dependencies: only pyarrow (available in most CMSSW releases).

Usage:
  python3 parquet_to_csv_cmssw.py input.parquet output.csv
  python3 parquet_to_csv_cmssw.py input.parquet  # defaults to input.csv

In CMSSW:
  cmsenv
  python3 parquet_to_csv_cmssw.py events.parquet events.csv
"""

import sys
import os

def main():
    # Parse arguments
    if len(sys.argv) < 2:
        print("Usage: python3 parquet_to_csv_cmssw.py <input.parquet> [output.csv]")
        print()
        print("Converts Apache Parquet files to CSV format.")
        print()
        print("Arguments:")
        print("  input.parquet    : Input parquet file (required)")
        print("  output.csv       : Output CSV file (optional, defaults to input.csv)")
        print()
        print("Example:")
        print("  python3 parquet_to_csv_cmssw.py data.parquet data.csv")
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2] if len(sys.argv) > 2 else infile.replace('.parquet', '.csv')

    # Check input file exists
    if not os.path.exists(infile):
        print(f"ERROR: Input file not found: {infile}")
        sys.exit(1)

    print("=" * 60)
    print("Parquet to CSV Converter (CMSSW compatible)")
    print("=" * 60)
    print(f"Input:  {infile}")
    print(f"Output: {outfile}")
    print()

    # Try to import pyarrow
    try:
        import pyarrow.parquet as pq
    except ImportError:
        print("ERROR: pyarrow not found.")
        print()
        print("Install with:")
        print("  pip install pyarrow")
        print()
        print("In CMSSW, you may need to use:")
        print("  python3 -m pip install --user pyarrow")
        sys.exit(1)

    # Read parquet
    print("Reading parquet file...")
    try:
        table = pq.read_table(infile)
    except Exception as e:
        print(f"ERROR: Failed to read parquet file: {e}")
        sys.exit(1)

    nrows = table.num_rows
    ncols = table.num_columns
    print(f"  Rows:    {nrows:,}")
    print(f"  Columns: {ncols}")
    print()

    # Show columns
    colnames = table.column_names
    print("Columns:")
    for i, col in enumerate(colnames[:10]):
        print(f"  [{i}] {col}")
    if ncols > 10:
        print(f"  ... and {ncols - 10} more")
    print()

    # Convert to CSV
    print("Converting to CSV...")
    try:
        # Convert to pandas (more robust for CSV writing)
        import pandas as pd
        df = table.to_pandas()
        df.to_csv(outfile, index=False)
    except ImportError:
        # Fallback: use pyarrow's CSV writer
        print("(pandas not available, using pyarrow CSV writer)")
        try:
            import pyarrow.csv as csv
            csv.write_table(table, outfile)
        except Exception as e:
            print(f"ERROR: Failed to write CSV: {e}")
            sys.exit(1)
    except Exception as e:
        print(f"ERROR: Failed to convert: {e}")
        sys.exit(1)

    # Verify output
    try:
        file_size_mb = os.path.getsize(outfile) / (1024.0 * 1024.0)
        print(f"Output file: {outfile}")
        print(f"File size:   {file_size_mb:.1f} MB")
        print()

        # Count lines in output
        with open(outfile, 'r') as f:
            lines = sum(1 for _ in f) - 1  # Subtract header
        print(f"Rows written: {lines:,}")

        if lines != nrows:
            print(f"WARNING: Row count mismatch (expected {nrows}, got {lines})")

    except Exception as e:
        print(f"WARNING: Could not verify output: {e}")

    print()
    print("=" * 60)
    print("Conversion complete!")
    print("=" * 60)
    print()
    print("Next step: Convert CSV to ROOT")
    print(f"  root -l -b -q 'csv_to_root_cmssw.C(\"{outfile}\", \"output.root\", \"Events\")'")
    print()

if __name__ == '__main__':
    main()
