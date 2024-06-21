#!/usr/bin/python3
import os,sys
import subprocess
import string
import utils as ut
from inspect import currentframe as here

def copyPlotsToDir(from_dir:str, to_dir:str, delete_preexisting_plots:bool, dp)->None:
    #make sure these directories exist.
    #dp is the debug printer
    ok = ut.assert_failPrints(os.path.exists(from_dir), f"Error! Directory {from_dir} doesn't exist. Unable to move plots out of it.")
    ok &= ut.assert_failPrints(os.path.exists(to_dir), f"Error! Directory {to_dir} doesn't exist. Unable to move plots into it.")
    if not ok:
        return
    if delete_preexisting_plots:
        dp.debug(here(),3,f"Deleting contents of {to_dir} before moving new files into it.") 
        os.system(f"rm {to_dir}/*.png 2>/dev/null")
    dp.debug(here(),4,f"Copying png files from {to_dir} to {to_dir}.") 
    os.system(f"cp {from_dir}/*.png {to_dir}/")
    dp.debug(here(),4,f"    finished copying.") 

#######################################################################################################################
def copy_plotsdir(tomlData) -> None:
    all_ok = True
    dp = ut.getDebugPrinter(tomlData)
    copy_to_google_drive, ok, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], all_ok, default_val=False)

    if copy_to_google_drive:
        plotsDir, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","plotDir"], all_ok, default_val="plots")
        plotDirGDrive, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","plotDirGDrive"], all_ok, default_val="plots")
        delete_preexisting_plots, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","delete_preexisting_plots"], all_ok, default_val=False)
        if all_ok:
            copyPlotsToDir(plotsDir, plotDirGDrive, delete_preexisting_plots, dp)
        else:
            db.debug(here(),1,"Error, config errors prevent copy_overlays")

#######################################################################################################################
def copy_scores(tomlData) -> None:
    dp = ut.getDebugPrinter(tomlData)
    copy_to_google_drive, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], True, default_val=False)
    if copy_to_google_drive:
        score_file, _, all_ok = ut.tomlGetSeq(tomlData, ["Scoring","score_file"], all_ok, default_val="scores.txt")
        score_dir, _, all_ok = ut.tomlGetSeq(tomlData, ["Scoring","score_dir"], all_ok, default_val="scores")
        score_dir_GDrive, _, all_ok = ut.tomlGetSeq(tomlData, ["Scoring","score_dir_GDrive"], all_ok, default_val="")

        if all_ok:
            subprocess.run(["cp", os.path.join(score_dir, score_file), os.path.join(score_dir_GDrive, score_file)])
            #todo make this *.txt
        else:
            db.debug(here(),1,"Error, config errors prevent copy_overlays")

#######################################################################################################################
def copy_overlays(tomlData) -> None:
    all_ok = True
    dp = ut.getDebugPrinter(tomlData)
    copy_to_google_drive, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], all_ok, default_val=False)
    if copy_to_google_drive:
        overlayDir, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","overlayDir"], all_ok, default_val="_final")
        overlayDirGDrive, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","overlayDirGDrive"], all_ok, default_val="_final")
        delete_preexisting_overlays, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","delete_preexisting_overlays"], all_ok, default_val=False)    
        if all_ok:
            copyPlotsToDir(overlayDir, overlayDirGDrive, delete_preexisting_overlays, dp)
        else:
            db.debug(here(),1,"Error, config errors prevent copy_overlays")

#######################################################################################################################
def copy_overlaysTR(tomlData) -> None:
    all_ok = True
    dp = ut.getDebugPrinter(tomlData)
    copy_to_google_drive, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], all_ok, default_val=False)
    if copy_to_google_drive:
        overlayDir, _, all_ok = ut.tomlGetSeq(tomlData, ["OverlayTR","overlayDir"], all_ok, default_val="_final")
        overlayDirGDrive, _, all_ok = ut.tomlGetSeq(tomlData, ["OverlayTR","overlayDirGDrive"], all_ok, default_val="_final")
        delete_preexisting_overlays, _, all_ok = ut.tomlGetSeq(tomlData, ["OverlayTR","delete_preexisting_overlays"], all_ok, default_val=False)    
        if all_ok:
            copyPlotsToDir(overlayDir, overlayDirGDrive, delete_preexisting_overlays, dp)
        else:
            db.debug(here(),1,"Error, config errors prevent copy_overlays")
#######################################################################################################################
def copyToGoogleDrive_data(tomlData) -> None:
    all_ok = True
    cfg_file_name = tomlData["tomlConfigFileName"]
    dp = ut.getDebugPrinter(tomlData)
    copy_to_google_drive,_,all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], all_ok, default_val=False)
    config_file_version, _,all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","config_file_version"], all_ok, default_val=-1)

    if copy_to_google_drive and all_ok:
        ut.check_config_file_version(
            cfg_file_name, 
            ut.tomlGetSeq(tomlData, ["ProcessCtrl","config_file_version"], True, default_val=-1)[0] 
            )

        copy_plotsdir(tomlData)
        copy_scores(tomlData)
        copy_overlays(tomlData)
    elif not all_ok:
        print(f"Error! config failure prevents copyToGoogleDirve.")
    else:
        db.debug(here(),3,f"copy_to_google_drive = false in {cfg_file_name}. No copy to Google Drive was performed.")


#######################################################################################################################
def copyToGoogleDrive(toml_config_file_path:str) -> None:
    tomlData,ok = ut.tomlLoad(toml_config_file_path)
    ut.assert_failExits(ok, f"Error! Failed to load toml config file {toml_config_file_path}")
    copyToGoogleDrive_data(tomlData)

#######################################################################################################################
if __name__ == "__main__":
    inlen = len(sys.argv)
    toml_config_file_path = "config.toml"
    if inlen > 1:
        toml_config_file_path = sys.argv[1]
    if inlen > 2:
        print(f"Warning! {sys.argv[0]} takes at most 1 input. {inlen-1} inputs were specified")

    copyToGoogleDrive(toml_config_file_path)
