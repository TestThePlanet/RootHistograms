#!/bin/bash

#Use: 
#./send.sh ["commit message"]

if [ $# -eq 1 ]; then
    commitMessage=$1
else
    commitMessage="Alterations committed `date` by Ilya"
fi

git add go.sh
git add config.toml
git add send.sh
git add Download_Google_Sheet.sh
git add CMSStyle.C
git add makeAllPlots.C
git add makeOverlays.py
git add make2Overlays.py
git add make2Overlays_toml.py
git add ../README.md
git commit -m "$commitMessage"
git push
