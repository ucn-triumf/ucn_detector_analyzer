#!/usr/bin/python3

# Convert all midas files to root files
# Derek Fujimoto
# Jul 2024

from glob import glob
import subprocess, os, shutil, sys
from multiprocessing import Pool, cpu_count

# settings
midas_dir = "/data3/ucn/midas_files/"
root_dir = "/data3/ucn/root_files/"
midas2root = '/home/ucn/online/ucn_detector_analyzer/midas2root.exe'
delete_midas2root_output = True

# check hostname
if 'HOSTNAME' not in os.environ.keys() or 'daq01' not in os.environ['HOSTNAME']:
    print('This script must be run on daq01, where midas2root.cxx is compiled. '+\
          'It will not work on daq04.')
    
    while True:
        inp = input('Force continue? y/[n]: ')
        
        if not inp or inp.lower()[0] == 'n':
            sys.exit()
        elif inp.lower()[0] == 'y':
            break
        else:
            print('Bad input. ', end='')

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
        return

    # change directories
    os.makedirs(root_dir, exist_ok=True)
    os.chdir(root_dir)

    # convert
    print(f'Converting {filepath}... ')
    printfile = f'run{run:0>8}_midas2root_output.txt'
    working_dir = os.path.dirname(midas2root)
    with open(printfile, 'w') as fid:
        subprocess.run(f'{midas2root} {filepath}', shell=True, 
                       stdout=fid, stderr=fid)

    # remove output text file
    if delete_midas2root_output:
        os.remove(printfile)

    print(f'Success: {filepath}')

# convert only the runs which have no matching root file
with Pool(max((cpu_count()-1, 1))) as pool:
    tuple(pool.imap_unordered(convert, midas_files))
print('Finished')
