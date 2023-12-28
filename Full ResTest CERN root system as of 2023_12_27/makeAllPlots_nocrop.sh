#!/bin/bash
folder="plots"
mkdir -p $folder

#root -q makeAllPlots.C+
root -b -q makeAllPlots.C+

sleep 1
echo "Plots made, do not crop"

# Perform the image crop using imagemagick convert
#find "$folder" -type f -name "*.png" | while read -r file; do
#    convert "$file" -crop -50x "$file"
#done
echo "End"
