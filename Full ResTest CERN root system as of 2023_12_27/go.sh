#!/bin/bash
echo "Doing git pull"
git stash
git pull
source makeAllPlots_nocrop.sh
echo "Making Overlays"
python3 make2Overlays.py


