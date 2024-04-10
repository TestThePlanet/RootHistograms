#!/bin/bash
echo "No Git Pull"
source makeAllPlots_nocrop.sh
echo "Making Overlays"
python3 make2Overlays.py


