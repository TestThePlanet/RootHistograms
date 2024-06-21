#!/usr/bin/env python   
import os,sys
import subprocess
import string
import utils as ut
from inspect import currentframe as here

"""
This version relies on the input toml config file 

To execute

./makeTROverlay.py [config_file.toml]

The config file input are optional. You can just put makeTROverlay.py in the directory above plots/ and run it with no inputs

./makeTROverlay.py 

In that case, config.toml is assumed. 

See default directories below. 
If plots and transphotos directories must exist, terminate early. 
If the output directory doesn't exist, it is created.

If you already have a toml config file loaded, call as 
import makeTROverlays
makeTROverlays.makeOverlay(tomlData)
"""

def makeOverlay(tomlData) -> None:
    ####################################################
    #Hard coded backup default values:

    #toml_config_file_path default set at bottom, following if __name__ ...
    plots_dir = "plots"
    transphotos_dir = "transparent_photos_front_TR"
    #transphotos_dir2 = "transparent_photos_inside_TR"
    output_dir = "_final_TR"
    Overlay_image_size = "400x300" 
    Overlay_top_location = "+1100+80" # 1000+50
    
    SinglePlotMode_enabled = False #bool 
    SinglePlotMode_which_one = "3M Aura 9210+" 
    Output_print_level = 3
    ####################################################

    #############Load params from TOML#######################################
    all_ok = True
    dp = ut.getDebugPrinter(tomlData)
    tomlOverlayCategory = "OverlayTR"

    Overlay_image_size, _, all_ok = ut.tomlGetSeq(tomlData, [tomlOverlayCategory,"image_size"], all_ok, default_val=Overlay_image_size)
    Overlay_top_location, _, all_ok = ut.tomlGetSeq(tomlData, [tomlOverlayCategory,"top_location"], all_ok, default_val=Overlay_top_location)
    
    output_dir, _, all_ok = ut.tomlGetSeq(tomlData, [tomlOverlayCategory,"overlayDir"], all_ok, default_val=output_dir)
    output_dir = os.path.join('.', output_dir)
    
    transphotos_dir, _, all_ok = ut.tomlGetSeq(tomlData, ["GoogleSheet","transphotos_dir_fronts"], all_ok, default_val=transphotos_dir)
    transphotos_dir = os.path.join('.', transphotos_dir)
    
    plots_dir, _, all_ok = ut.tomlGetSeq(tomlData, ["Output","plotDir"], all_ok, default_val=plots_dir)
    plots_dir = os.path.join('.',plots_dir)
    
    SinglePlotMode_enabled, _, all_ok = ut.tomlGetSeq(tomlData, ["SinglePlotMode","single_plot_mode_enabled"], all_ok, default_val=SinglePlotMode_enabled)
    SinglePlotMode_which_one , _, all_ok = ut.tomlGetSeq(tomlData, ["SinglePlotMode","which_one"], all_ok, default_val=SinglePlotMode_which_one)
    ######################################################################################
    #Check that the directories exist.
    ut.assert_failExits(os.path.exists(plots_dir), f"Error! The plots directory does not exist {plots_dir}")
    ut.assert_failExits(os.path.exists(transphotos_dir), f"Error! The first transparent plots directory does not exist {transphotos_dir}")
    ut.ensure_dir(output_dir)
    
    ut.assert_failPrints(all_ok, f"Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")
    
    def backslashify_brackets(fname, backslashSpaces = False): #str -> str
        fname = fname.replace(')', '\)')
        fname = fname.replace('(', '\(')
        fname = fname.replace('[', '\[')
        fname = fname.replace(']', '\]')
        fname = fname.replace('{', '\{')
        fname = fname.replace('}', '\}')
        if backslashSpaces:
            fname = fname.replace(' ', '\ ')
        return fname 
    
    #Read in lists of images in the plots and transphotos dirs
    plots      = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {plots_dir}/*.png")]
    transphotos = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir}/*.png")]
    
    SinglePlotMode_which_one = backslashify_brackets(SinglePlotMode_which_one.strip(), backslashSpaces = True) + ".png"
    
    i = 0
    cnt_1st = 0
    for plot in plots:
        plot_stem = plot[len(plots_dir)+9:]
    
        if SinglePlotMode_enabled and SinglePlotMode_which_one != plot_stem:
            continue
        elif SinglePlotMode_enabled:
            dp.debug(here(),2,"found",SinglePlotMode_which_one)
    
        plotmorph = os.path.join(transphotos_dir, plot_stem) 

        if plotmorph in transphotos:
            plotmorphout = os.path.join(output_dir, plot[len(plots_dir)+1:] )

            make_command_part1 = f"convert -quiet {plotmorph} " #TODO add silencing, -quiet might not be right.
            make_command_part2 = f"\( {plot} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite "
            make_command_part4 = f"{plotmorphout}" 
    
            make_command = make_command_part1 + make_command_part2 + make_command_part4 
            dp.print(3,"Making overlay",plotmorphout)
            subprocess.run(make_command, shell=True) 
            cnt_1st  += 1
        i += 1
    
    dp.print(2,f"\nOverlay Process Report")
    dp.print(2,f"    Out of {len(plots)} histograms in {plots_dir}")
    dp.print(2,f"    {cnt_1st} only had a match in {transphotos_dir}")
    dp.print(2,f"    {len(plots) - cnt_1st} had no matches anywhere.")
    dp.print(2,f"    {len(transphotos) - cnt_1st} unused files in {transphotos_dir}")

def makeOverlay_toml(toml_config_file_path:str) -> None:
    tomlData,ok = ut.tomlLoad(toml_config_file_path) 
    ut.assert_failExits(ok, f"Error! Failed to load toml config file {toml_config_file_path}")
    makeOverlay(tomlData) 

if __name__ == "__main__":
    inlen = len(sys.argv)
    toml_config_file_path = "config.toml"
    if inlen > 1:
        toml_config_file_path = sys.argv[1]
    #too many inputs
    if inlen > 2:
        print(f"Warning! This takes at most 1 input. {inlen-1} inputs were specified")

    makeOverlay_toml(toml_config_file_path)
