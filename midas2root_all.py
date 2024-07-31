#!/usr/bin/python3

# Convert all midas files to root files
# Derek Fujimoto
# Jul 2024

from glob import glob
import subprocess, os, shutil
from multiprocessing import Pool, cpu_count

# settings
midas_dir = "/data2/ucn/midas_files/"
root_dir = "/data2/ucn/root_files/"
midas2root = '/home/ucn/online/ucn_detector_analyzer/midas2root.exe'

# get list of all files
midas_files = glob(os.path.join(midas_dir, '*.mid.gz'))
root_files = glob(os.path.join(root_dir, 'ucn_run*.root'))

# get run numbers from filenames
def get_midas_run(filename):
    filename = os.path.basename(filename)
    filename = os.path.splitext(filename)[0]
    filename = os.path.splitext(filename)[0]
    return int(filename.replace('run', ''))

def get_root_run(filename):
    filename = os.path.basename(filename)
    filename = os.path.splitext(filename)[0]
    return int(filename.replace('ucn_run_', ''))

midas_runs = [get_midas_run(m) for m in midas_files]
root_runs = [get_root_run(r) for r in root_files]

# define conversion function
def convert(filepath):

    # check
    run = get_midas_run(filepath)
    if run in root_runs:
        print(f'Skipping {filepath}. Root file exists')
        return

    # convert
    subprocess.run(f'{midas2root} {filepath}', shell=True, capture_output=True)
    newfile = f'ucn_run_{run:0>8}.root'

    # move
    os.makedirs(root_dir, exist_ok=True)
    newfilepath = os.path.join(root_dir, newfile)
    shutil.move(newfile, newfilepath)

    print(f'Success: {filepath} --> {newfilepath}')

# convert only the runs which have no matching root file
with Pool(max((cpu_count()-1, 1))) as pool:
    tuple(pool.imap_unordered(convert, midas_files))
print('Finished')
