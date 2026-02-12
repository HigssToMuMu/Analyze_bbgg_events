#!/usr/bin/env python3
"""
Convert parquet file to flat ROOT ntuple using pandas and uproot.

NOTE: The resulting ROOT file can be read with uproot (Python) without issues,
but the native ROOT interpreter (C++) in ROOT 6.38+ may have compatibility
issues. For interactive ROOT analysis, use uproot-based Python scripts instead.

Usage:
  python3 convert_parquet.py          # Creates events.root from events.parquet

Python example (with uproot):
  import uproot
  with uproot.open('events.root') as f:
      tree = f['Events']
      mass = tree['mass'].array()
"""
import pyarrow.parquet as pq
import uproot
import pandas as pd
import numpy as np

infile = '/Users/cmorgoth/git_repos/Analyze_bbgg_events/parquet_example/events.parquet'
outfile = '/Users/cmorgoth/git_repos/Analyze_bbgg_events/parquet_example/events.root'

# Read entire parquet as pandas DataFrame
print(f'Reading {infile}...')
df = pd.read_parquet(infile)
print(f'  {df.shape[0]} rows, {df.shape[1]} columns')

# Convert to dict of numpy arrays, ensuring proper dtypes
print('Converting to numpy arrays...')
branches = {}
for col in df.columns:
    dtype = df[col].dtype
    if dtype == 'bool':
        branches[col] = df[col].astype(np.bool_).values
    elif dtype == 'uint8':
        branches[col] = df[col].astype(np.uint8).values
    elif dtype == 'int64':
        branches[col] = df[col].astype(np.int64).values
    elif dtype == 'float32':
        branches[col] = df[col].astype(np.float32).values
    elif dtype == 'float64':
        branches[col] = df[col].astype(np.float64).values
    else:
        branches[col] = df[col].values

# Write to ROOT
print(f'Writing to {outfile}...')
with uproot.recreate(outfile) as f:
    f['Events'] = branches

print('Done!')

# Verify
with uproot.open(outfile) as f:
    tree = f['Events']
    print(f'Verification: {tree.num_entries} entries, {len(tree.keys())} branches')
    sample = tree['mass'].array()[:5]
    print(f'Sample values (mass): {sample}')
