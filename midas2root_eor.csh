#!/bin/tcsh
# midas2root script
# T. Lindner

# Go to right output directory
#cd /data/ucn/root_files/

# Get current filename
set Filename=`odbedit -c 'ls "/Logger/Channels/0/Settings/Current filename"' | awk -F "  +" '{print $2}'`

# Set midas file directory
set MidasDir=/data3/ucn/midas_files
set RootDir=/data3/ucn/root_files
set WorkDir=/home/ucn/online/ucn_detector_analyzer

# Do the conversion
$WorkDir/midas2root.exe $MidasDir/$Filename

# get latest file made
set NewFile=`ls -t $WorkDir/*.root | head -n1`

# move the file
mv $NewFile $RootDir/
