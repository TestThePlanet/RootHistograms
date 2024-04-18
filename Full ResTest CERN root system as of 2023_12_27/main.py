#!/usr/bin/python3
import os,sys
import subprocess
import string
import urllib.request
import time
import utils as ut

def check_config_file_version(config_file_version:int):
    #Check that the supplied config file has a version that is compatibe with this codebase 
    required_min_config_file_version = 1
    ut.assert_failExits( config_file_version >= required_min_config_file_version,\
            f"Error! Config file {toml_config_file} is version {config_file_version } which is requires at least version {expected_config_file_version }")

#######################################################################################################################

def check_config_files(config_data_list):
    #return same_dir_conflict, deletion_conflict 
    #if everything is ok, returns False, False

    same_dir_conflict = False
    deletion_conflict = False
    plots_dir_dict = {}  #maps dirs to config file names
    overlay_dir_dict = {}#maps dirs to config file names
    for data in config_data_list:
        this_config_file_name = data["tomlConfigFileName"]
        if ut.tomlGetSeq(data, ["Output","run_makeAllPlots_C"], None, default_val=True)[0]:
            plots_dir, _ = ut.tomlGetSeq(data, ["Output","plotDir"], None, default_val="plots")
            if plots_dir in plots_dir_dict:

                if ut.tomlGetSeq(data, ["Output","delete_preexisting_plots"], None, default_val=False)[0]: #if deleting preivous output
                    print(f"Error! Config file deletion conflict: calling {this_config_file_name} will **delete** the plots in {plots_dir} created by {plots_dir_dict[plots_dir]}")
                    deletion_conflict = True
                else:
                    print(f"Warning! Config file overwrite conflict: calling {this_config_file_name} will write plots in {plots_dir} likely overwriting plots created by {plots_dir_dict[plots_dir]}")
                    same_dir_conflict = True
            else: 
                plots_dir_dict[plots_dir] = this_config_file_name 

        if ut.tomlGetSeq(data, ["Overlay","run_make2Overlays_toml_py"], None, default_val=True)[0]:
            overlayDir, _ = ut.tomlGetSeq(data, ["Overlay","overlayDir"], None, default_val="_final")
            if overlayDir in overlay_dir_dict:
                this_config_file_name = data["tomlConfigFileName"]
                if ut.tomlGetSeq(data, ["Overlay","delete_preexisting_overlays"], None, default_val=False)[0]: #if deleting preivous output
                    print(f"Error! Config file deletion conflict: calling {this_config_file_name} will **delete** the plots in {overlayDir} created by {overlay_dir_dict[overlayDir]}")
                    deletion_conflict = True
                else:
                    print(f"Warning! Config file overwrite conflict: calling {this_config_file_name} will write plots in {overlayDir} likely overwriting plots created by {overlay_dir_dict[overlayDir]}")
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
    files = [os.path.join(plots_dir, file) for file in os.listdir(plots_dir)]
    if files:
        latest_file = sorted(files, key=os.path.getmtime)[-1]
        subprocess.Popen(["eog", latest_file], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL, start_new_session=True)
    
#######################################################################################################################

def process_config(toml_data):
    #Takes the parsed nested dictionary from the toml config file
        
    config_file_version, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","config_file_version"], all_ok, default_val=-1)
    check_config_file_version(config_file_version)

    delete_preexisting_plots, _, all_ok = ut.tomlGetSeq(data, ["Output","delete_preexisting_plots"], all_ok, default_val=False)
    plots_dir, _, all_ok = ut.tomlGetSeq(data, ["Output","plotDir"], all_ok, default_val="plots")

    delete_preexisting_overlays, _, all_ok = ut.tomlGetSeq(data, ["Overlay","delete_preexisting_overlays"], all_ok, default_val=False)
    overlayDir, _, all_ok = ut.tomlGetSeq(data, ["Overlay","overlayDir"], all_ok, default_val="_final")
    single_plot_mode_enabled, _, all_ok = ut.tomlGetSeq(data, ["SinglePlotMode","single_plot_mode_enabled"], all_ok, default_val=False)
    run_makeAllPlots_C, _, all_ok = ut.tomlGetSeq(data, ["Output","run_makeAllPlots_C"], all_ok, default_val=True)
    run_make2Overlays_toml_py, _, all_ok = ut.tomlGetSeq(data, ["Overlay","run_make2Overlays_toml_py"], all_ok, default_val=True)
    open_main_eog, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","open_main_eog"], all_ok, default_val=True)
    open_overlay_eog, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","open_overlay_eog"], all_ok, default_val=True)
    if not all_ok:
        print(f"Warning: Problems were encountered while loading toml config file {toml_config_file}")

    ##########################
    os.makedirs(plots_dir, exist_ok=True)

    if run_makeAllPlots_C:
        if delete_preexisting_plots:
            print(f"Deleting contents of {plots_dir} since Output.delete_preexisting_plots = true in {toml_config_file}")
            for file_name in os.listdir(plots_dir):
                if file_name.endswith(".png"):
                    os.remove(os.path.join(plots_dir, file_name) )

        print( "Making Plots" )
        if single_plot_mode_enabled:
            subprocess.run( ["root", "-q", f'makeAllPlots.C+(\"{toml_config_file}\")'] )
        else: 
            subprocess.run( ["root", "-b", "-q", f'makeAllPlots.C+(\"{toml_config_file}\")'] )
    
        time.sleep(1)
        print("Plots made")
    
        if open_main_eog:
            display_latest_eog(plots_dir)

    if run_make2Overlays_toml_py:
        if delete_preexisting_overlays:
            print(f"Deleting contents of {overlayDir} since Overlay.delete_preexisting_overlays = true in {toml_config_file}")
            for file_name in os.listdir(overlayDir):
                if file_name.endswith(".png"):
                    os.remove(os.path.join(plots_dir, file_name) )

        print( "Making Overlays" )
        subprocess.run( [sys.executable, "make2Overlays_toml.py", toml_config_file] ) #TODO this is stupid, make it a funciton
        if open_overlay_eog:
            display_latest_eog(overlayDir)

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

############Handle Commandline input##############

toml_config_files = ["config.toml"]
if len(sys.argv) > 1:
    toml_config_files = sys.argv[1:]

#########Load params from TOML[0]#################
data, all_ok = ut.tomlLoad(toml_config_files[0], exit_on_fail = True) 
all_load_ok = all_ok

config_file_version, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","config_file_version"], all_ok, default_val=-1)
check_config_file_version(config_file_version)

do_git_pull, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","do_git_pull"], all_ok, default_val=True)

GoogleSheet_redownload, _, all_ok = ut.tomlGetSeq(data, ["GoogleSheet","redownload"], all_ok, default_val=True)


ut.assert_failPrints(all_ok, "Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")

######## Manage Git Pull ##########################
if do_git_pull:
    #TODO Here's a dangerous contradiciton. Git pull may overwrite the config file and change behavior mid execution. 
    print( "Doing git pull" )
    subprocess.run(["git", "stash"])
    subprocess.check_call(["git", "pull"])
##########makeAllPlots.sh#############################################
if GoogleSheet_redownload:
    ut.Download_Google_Sheet(data)
    #subprocess.run( [sys.executable, "Download_Google_Sheet.py"] )

#### Run config file inputs
config_data_list = [data,]
remaining_config_data,ok = ut.tomlLoadAll(toml_config_file[1:])
all_load_ok &= ok
config_data_list = config_data_list + remaining_config_data 

ut.assert_failExits(all_load_ok, "Error! Some of the given config files could not be loaded. Exiting!")

same_dir_conflict, deletion_conflict = check_config_files(config_data_list)
if deletion_conflict:
    print("Exiting!")
    sys.exit()
elif same_dir_conflict
    while(True):
        user_responce = input("Do you want to continue Y/N?").lower()
        if user_responce[0] == 'y':
            break
        elif user_responce[0] == 'n':
            sys.exit()

#Process all config files
for dat in config_data_list: 
    process_config(dat)

