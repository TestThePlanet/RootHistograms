#!/bin/bash

#Use: 
#./send.sh ["commit message"]

if [ $# -eq 1 ]; then
    commitMessage=$1
else
    commitMessage=echo "Alterations committed `date` by Ilya"
fi

if [ $(grep -c "^[^/*]*bool single_plot_mode_enabled[^/*]*disable" makeAllPlots.C) -eq 1]; then
    git add go.sh
    git add send.sh
    git add Download_Google_Sheet.sh
    git add CMSStyle.C
    git add makeAllPlots.C
    git add makeAllPlots.sh
    git add makeAllPlots_nocrop.sh
    git add makeOverlays.py
    git add make2Overlays.py
    git commit -m "$commitMessage"
    git push
else
    echo "Commit refu"
fi


