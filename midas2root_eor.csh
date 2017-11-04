#!/bin/tcsh
# midas2root script
# T. Lindner

# Go to right output directory
cd /data/ucn/root_files/

# Get current filename
set Filename=`odbedit -c 'ls "/Logger/Channels/0/Settings/Current filename"' | awk -F "  +" '{print $2}'`

# Set midas file directory
set MidasDir=/data/ucn/midas_files/

# Do the conversion
/home/ucn/online/ucn_detector_analyzer/midas2root.exe $MidasDir/$Filename

