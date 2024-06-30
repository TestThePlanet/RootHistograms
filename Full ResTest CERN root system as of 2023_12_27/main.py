#!/usr/bin/python3
import os,sys
import subprocess
import string
import urllib.request
import time
import utils as ut
from inspect import currentframe as here
import make2Overlays_toml
import makeTROverlay
import copyToGoogleDrive as cgd

def check_config_files(tomlConfigDataList):
    #return same_dir_conflict, deletion_conflict 
    #if everything is ok, returns False, False

    same_dir_conflict = False
    deletion_conflict = False
    plots_dir_dict = {}  #maps dirs to config file names
    overlay_dir_dict = {}#maps dirs to config file names
    overlayTR_dir_dict = {}#maps dirs to config file names
    for tomlData in tomlConfigDataList:
        this_config_file_name = tomlData["tomlConfigFileName"]

        if ut.tomlGetSeq(tomlData, ["Output","run_makeAllPlots_C"], None, default_val=True)[0]:
            plotsDir, _ = ut.tomlGetSeq(tomlData, ["Output","plotDir"], None, default_val="plots")
            if plotsDir in plots_dir_dict:

                if ut.tomlGetSeq(tomlData, ["Output","delete_preexisting_plots"], None, default_val=False)[0]: #if deleting preivous output
                    print(f"Error! Config file deletion conflict: calling {this_config_file_name} will **delete** the plots in {plotsDir} created by {plots_dir_dict[plotsDir]}")
                    deletion_conflict = True
                else:
                    print(f"Warning! Config file overwrite conflict: calling {this_config_file_name} will write plots in {plotsDir} likely overwriting plots created by {plots_dir_dict[plotsDir]}")
                    same_dir_conflict = True
            else: 
                plots_dir_dict[plotsDir] = this_config_file_name 

        if ut.tomlGetSeq(tomlData, ["Overlay","runOverlays"], None, default_val=True)[0]:
            overlayDir, _ = ut.tomlGetSeq(tomlData, ["Overlay","overlayDir"], None, default_val="_final")
            if overlayDir in overlay_dir_dict:
                this_config_file_name = tomlData["tomlConfigFileName"]
                if ut.tomlGetSeq(tomlData, ["Overlay","delete_preexisting_overlays"], None, default_val=False)[0]: #if deleting preivous output
                    print(f"Error! Config file deletion conflict: calling {this_config_file_name} will **delete** the plots in {overlayDir} created by {overlay_dir_dict[overlayDir]}")
                    deletion_conflict = True
                else:
                    print(f"Warning! Config file overwrite conflict: calling {this_config_file_name} will write plots in {overlayDir} likely overwriting plots created by {overlay_dir_dict[overlayDir]}")
                    same_dir_conflict = True
            else:
                overlay_dir_dict[overlayDir] = this_config_file_name 

        if ut.tomlGetSeq(tomlData, ["OverlayTR","runOverlays"], None, default_val=True)[0]:
            overlayDir, _ = ut.tomlGetSeq(tomlData, ["OverlayTR","overlayDir"], None, default_val="_final")
            if overlayDir in overlay_dir_dict:
                this_config_file_name = tomlData["tomlConfigFileName"]
                if ut.tomlGetSeq(tomlData, ["OverlayTR","delete_preexisting_overlays"], None, default_val=False)[0]: #if deleting preivous output
                    print(f"Error! Config file deletion conflict: calling {this_config_file_name} will **delete** the plots in {overlayDir} created by {overlayTR_dir_dict[overlayDir]}")
                    deletion_conflict = True
                else:
                    print(f"Warning! Config file overwrite conflict: calling {this_config_file_name} will write plots in {overlayDir} likely overwriting plots created by {overlayTR_dir_dict[overlayDir]}")
                    same_dir_conflict = True
            else:
                overlay_dir_dict[overlayDir] = this_config_file_name 
    return same_dir_conflict, deletion_conflict 

#######################################################################################################################
def display_latest_eog(plots_dir:str):
    try:
        # Check if eog is installed
        subprocess.run(["eog", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except FileNotFoundError:
        print("eog is not installed. On WSLg, install it with sudo apt-get -y install eog")
        return
    os.makedirs(plots_dir, exist_ok=True)
    files = [os.path.join(plots_dir, file) for file in os.listdir(plots_dir)]
    if files:
        latest_file = sorted(files, key=os.path.getmtime)[-1]
        subprocess.Popen(["eog", latest_file, "&"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL, start_new_session=True)

#######################################################################################################################
def process_config(tomlData):
    #Takes the parsed nested dictionary from the toml config file

    this_config_file_name = tomlData["tomlConfigFileName"]

    all_ok = True
    Output_print_level, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","print_level"], all_ok, default_val=3)
    dp = ut.DebugPrinter(Output_print_level)
    dp.debug(here(),1,"running process_config()") 

    config_file_version, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","config_file_version"], all_ok, default_val=-1)
    ut.check_config_file_version(this_config_file_name, config_file_version)

    single_plot_mode_enabled, _, all_ok = ut.tomlGetSeq(tomlData, ["SinglePlotMode","single_plot_mode_enabled"], all_ok, default_val=False)
    copy_to_google_drive, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], all_ok, default_val=False)

    delete_preexisting_plots, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","delete_preexisting_plots"], all_ok, default_val=False)

    run_makeAllPlots_C, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","run_makeAllPlots_C"], all_ok, default_val=True)
    make_scores, _, all_ok = ut.tomlGetSeq(tomlData, ["Scoring","make_scores"], all_ok, default_val=False)
    plotsDir, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","plotDir"], all_ok, default_val="plots")
    open_main_eog, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","open_main_eog"], all_ok, default_val=True)

    run_make2Overlays_toml_py, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","runOverlays"], all_ok, default_val=True)
    overlayDir, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","overlayDir"], all_ok, default_val="_final")
    delete_preexisting_overlays, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","delete_preexisting_overlays"], all_ok, default_val=False)

    run_makeTROverlays_py, _, all_ok = ut.tomlGetSeq(tomlData, ["OverlayTR","runOverlays"], all_ok, default_val=True)
    overlayDirTR, _, all_ok = ut.tomlGetSeq(tomlData, ["OverlayTR","overlayDir"], all_ok, default_val="_final")
    delete_preexisting_overlaysTR, _, all_ok = ut.tomlGetSeq(tomlData, ["OverlayTR","delete_preexisting_overlays"], all_ok, default_val=False)

    open_overlay_eog, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","open_overlay_eog"], all_ok, default_val=True)

    if not all_ok:
        print(f"Warning: Problems were encountered while loading toml config file {this_config_file_name}")

    ##########################
    dp.debug(here(),4,f"Making dirs: {plotsDir}")
    os.makedirs(plotsDir, exist_ok=True)

    if run_makeAllPlots_C:
        if delete_preexisting_plots:
            dp.debug(here(),2,f"Deleting contents of {plotsDir} since Output.delete_preexisting_plots = true in {this_config_file_name}")
            os.system(f"rm {plotsDir}/*.png 2>/dev/null")
            dp.debug(here(),4,f"    finished deleting plots in {plotsDir}")

        dp.debug(here(),3,"Making Plots") 
        if single_plot_mode_enabled:
            subprocess.run( ["root", "-l", f'makeAllPlots.C+(\"{this_config_file_name}\")'] )
        else: 
            subprocess.run( ["root", "-l", "-b", "-q", f'makeAllPlots.C+(\"{this_config_file_name}\")'] )
    
        time.sleep(1)
        dp.debug(here(),3,"Plots made")
    
        if open_main_eog:
            display_latest_eog(plotsDir)

    if run_make2Overlays_toml_py:
        if delete_preexisting_overlays:
            dp.debug(here(),2,f"Deleting contents of {overlayDir} since Overlay.delete_preexisting_overlays = true in {this_config_file_name}")
            os.system(f"rm {overlayDir}/*.png 2>/dev/null")
            dp.debug(here(),4,f"    finished deleting plots in {overlayDir}")

        dp.debug(here(),1,"Making Overlays")
        make2Overlays_toml.makeOverlay(tomlData)

    if run_makeTROverlays_py:
        if delete_preexisting_overlaysTR:
            dp.debug(here(),2,f"Deleting contents of {overlayDirTR} since OverlayTR.delete_preexisting_overlays = true in {this_config_file_name}")
            os.system(f"rm {overlayDirTR}/*.png 2>/dev/null")
            dp.debug(here(),4,f"    finished deleting plots in {overlayDirTR}")
        makeTROverlay.makeOverlay(tomlData)
        if open_overlay_eog: #TODO put this inside makeOverlay
            display_latest_eog(overlayDir)

    if run_makeAllPlots_C:
        cgd.copy_plotsdir(tomlData)
        if make_scores:
            cgd.copy_scores(tomlData)
    if run_make2Overlays_toml_py:
        cgd.copy_overlays(tomlData)
    if run_makeTROverlays_py:
        cgd.copy_overlaysTR(tomlData)

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

############Handle Commandline input##############

toml_config_files = ["config.toml"]
if len(sys.argv) > 1:
    toml_config_files = sys.argv[1:]

############ COPY arg files ######################
toml_config_files_active = ['.'+fn for fn in toml_config_files]

all_exist = True
for orig in toml_config_files:
    is_here = os.path.exists(orig)
    if not is_here:
        print("The config file ",orig," doesn't exist. Exiting")
    all_exist &= is_here

if all_exist:
    for orig, active in zip(toml_config_files, toml_config_files_active):
        try:
            subprocess.run(["cp", orig, active], check=True)
        except subprocess.CalledProcessError:
            print(f"Warning, unable to copying '{orig}' to '{active}'")
            all_exist = False

if not all_exist:
    sys.exit()

#########Load params from TOML[0]#################
tomlData, all_ok = ut.tomlLoad(toml_config_files_active[0], exit_on_fail = True) 
all_load_ok = all_ok

config_file_version,  _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","config_file_version"], all_ok, default_val=-1)
ut.check_config_file_version(toml_config_files_active[0], config_file_version)

do_git_pull,          _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","do_git_pull"], all_ok, default_val=True)
ask_on_git_diff,      _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","ask_on_git_diff"], all_ok, default_val=True)
git_diff_verbosity,   _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","ask_on_git_diff"], all_ok, default_val="info")
GoogleSheet_redownload,_,all_ok = ut.tomlGetSeq(tomlData, ["GoogleSheet","redownload"], all_ok, default_val=True)

ut.assert_failPrints(all_ok, "Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")

######## Manage Git Pull ##########################
is_online = ut.is_online()
if do_git_pull and is_online:
    print( "Doing git pull" )
    subprocess.run(["git", "stash","-q"])
    subprocess.check_call(["git", "pull", "-q"])
elif is_online:
    print("Skipping git pull")
else:
    print("No web connection -> skipping git pull")

########## Note what the git pull changed, #############################################
for i, (orig, active) in enumerate(zip(toml_config_files, toml_config_files_active)):
    diff_arg_list = ["diff","--color", "--text", "--ignore-space-change", "--ignore-blank-lines", "--ignore-trailing-space", "--ignore-tab-expansion", "--ignore-blank-lines", active, orig]
    if subprocess.run(diff_arg_list , stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode != 0:
        #the files differ
        if ask_on_git_diff:
            subprocess.run(diff_arg_list , stdout = subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"USER INPUT REQUIRED: the git pull updated the toml config file {orig}. Options:")
            print("C: Continue with the pre-git-pull config file")
            print("R: Replace the config file as run with the newly pulled file")
            print("Q: Quit and restore local area from git stash")
            while(True):
                user_responce = input("Selection: ").lower()
                if user_responce[0] == 'c':
                    break
                elif user_responce[0] == 'r':
                    if i == 0:
                        toml_config_files_active[i] = orig

                        #Reload the current file as the git-pulled file
                        tomlData, all_ok2 = ut.tomlLoad(toml_config_files_active[0], exit_on_fail = True) 
                        config_file_version, _, all_ok2 = ut.tomlGetSeq(tomlData, ["ProcessCtrl","config_file_version"], all_ok2, default_val=-1)
                        ut.check_config_file_version(toml_config_files_active[0], config_file_version)
                        GoogleSheet_redownload, _, _ = ut.tomlGetSeq(tomlData, ["GoogleSheet","redownload"], all_ok2, default_val=True)
                        if not all_ok2:
                            print("Warning! Unable to read some paramters from the pulled file. Reverting to as-run config file.")
                            toml_config_files_active[i] = active

                            #Reload the current file as actively used file
                            tomlData, all_ok3 = ut.tomlLoad(toml_config_files_active[0], exit_on_fail = True) 
                            config_file_version, _, _ = ut.tomlGetSeq(tomlData, ["ProcessCtrl","config_file_version"], all_ok3, default_val=-1)
                            ut.check_config_file_version(toml_config_files_active[0], config_file_version)
                            GoogleSheet_redownload, _, _ = ut.tomlGetSeq(tomlData, ["GoogleSheet","redownload"], all_ok3, default_val=True)
                            if not all_ok3:
                                print(f"Ok, I quit, the as-run file {active} is now not readable. Exiting")
                                sys.exit()
                    else:
                        toml_config_files_active[i] = toml_config_files[i]
                    break
                elif user_responce[0] == 'q':
                    subprocess.run(["git", "stash","apply"])
                    sys.exit()
        elif git_diff_verbosity != "silent" or ask_on_git_diff:
            if git_diff_verbosity == "verbose":
                print(f"Diff between pre- and post- git pull for {orig}")
                subprocess.run(diff_arg_list , stdout = subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"INFO: the git pull updated the toml config file {orig}")
##########makeAllPlots.sh#############################################
if GoogleSheet_redownload:
    ut.Download_Google_Sheet(tomlData)
else:
    print("Skipping redownload of the google sheet")

#### Run config file inputs
tomlConfigDataList = [tomlData,]
remaining_config_data,ok = ut.tomlLoadAll(toml_config_files_active[1:])
all_load_ok &= ok
tomlConfigDataList = tomlConfigDataList + remaining_config_data 

ut.assert_failExits(all_load_ok, "Error! Some of the given config files could not be loaded. Exiting!")

same_dir_conflict, deletion_conflict = check_config_files(tomlConfigDataList)
if deletion_conflict:
    print("Exiting!")
    sys.exit()
elif same_dir_conflict:
    while(True):
        user_responce = input("Do you want to continue Y/N?").lower()
        if user_responce[0] == 'y':
            break
        elif user_responce[0] == 'n':
            sys.exit()

#Process all config files
for tomlDat in tomlConfigDataList: 
    process_config(tomlDat)