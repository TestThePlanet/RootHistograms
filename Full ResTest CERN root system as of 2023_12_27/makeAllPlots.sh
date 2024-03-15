#!/bin/bash
folder="plots"
mkdir -p $folder

python Download_Google_Sheet.py

echo "Making Plots"
root -b -q makeAllPlots.C+

sleep 1
echo "Plots made, do not crop"

# Perform the image crop using imagemagick convert
#find "$folder" -type f -name "*.png" | while read -r file; do
#    convert "$file" -crop -50x "$file"
#done
if command -v eog > /dev/null; then
    eog plots/"$(ls -t plots/| head -n1)"
else
    echo "eog is not installed. On WSLg, do sudo apt-get -y install eog"
fi
echo "End"
