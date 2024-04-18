#!/usr/bin/python3
import os,sys,
import subprocess
import string
import urllib.request
import time
import utils as ut

def display_latest_eog(plots_dir:string):
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
    
############################################################################

def process_config(toml_config_file:string ="config.toml", exit_on_fail:bool = True):
    data, all_ok  = ut.tomlLoad(toml_config_file, exit_on_fail) 


    delete_preexisting_plots, _, all_ok = ut.tomlGetSeq(data, ["Output","delete_preexisting_plots"], default_val=false, all_ok)
    plots_dir, _, all_ok = ut.tomlGetSeq(data, ["Output","plotDir"], default_val="plots", all_ok)

    delete_preexisting_overlays, _, all_ok = ut.tomlGetSeq(data, ["Overlay","delete_preexisting_overlays"], default_val=false, all_ok)
    overlayDir, _, all_ok = ut.tomlGetSeq(data, ["Overlay","overlayDir"], default_val="_final", all_ok)
    single_plot_mode_enabled, _, all_ok = ut.tomlGetSeq(data, ["SinglePlotMode","single_plot_mode_enabled"], default_val=False, all_ok)
    run_makeAllPlots_C, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","run_makeAllPlots_C"], default_val=True, all_ok)
    run_make2Overlays_toml_py, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","run_make2Overlays_toml_py"], default_val=True, all_ok)
    open_main_eog, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","open_main_eog"], default_val=True, all_ok)
    open_overlay_eog, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","open_overlay_eog"], default_val=True, all_ok)
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
        subprocess.run( [sys.executable, "make2Overlays_toml.py", toml_config_file] ) #TODO this is stupid, fix it
        if open_overlay_eog:
            display_latest_eog(overlayDir)

############Handle Commandline input########################################
toml_config_files = ["config.toml"]
if len(sys.argv) > 1:
    toml_config_files = sys.argv[1:]

#########Load params from TOML[0]###########################################
data, all_ok = ut.tomlLoad(toml_config_files[0], exit_on_fail = True) 

do_git_pull, _, all_ok = ut.tomlGetSeq(data, ["ProcessCtrl","do_git_pull"], True, all_ok)
GoogleSheet_redownload, _, all_ok = ut.tomlGetSeq(data, ["GoogleSheet","redownload"], True, all_ok)

ut.assert_failPrints(all_ok, "Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")

####################################################
if do_git_pull:
    print( "Doing git pull" )
    subprocess.run(["git", "stash"])
    subprocess.check_call(["git", "pull"])
##########makeAllPlots.sh#############################################
if GoogleSheet_redownload:
    ut.Download_Google_Sheet(data)
    #subprocess.run( [sys.executable, "Download_Google_Sheet.py"] )

#### Run config file inputs
process_config(toml_config_file, exit_on_fail = True)
for toml_config_file in toml_config_file[1:]:
    process_config(toml_config_file, exit_on_fail = False)
