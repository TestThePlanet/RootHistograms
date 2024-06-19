#!/usr/bin/env python   
import os,sys
import subprocess
import string
import utils as ut

"""
This version relies on the input toml config file 

To execute

./make2Overlays.py [config_file.toml]

The config file input are optional. You can just put makeOverlays.py in the directory above plots/ and run it with no inputs

./make2Overlays.py 

In that case, config.toml is assumed. 

See default directories below. 
If plots and transphotos directories must exist, terminate early. 
If the output directory doesn't exist, it is created.
"""

def make2Overlays_tomldata(tomlData) -> None:
    ####################################################
    #Hard coded backup default values:

    #toml_config_file_path default set at bottom, 
    #following if __name__ ...
    plots_dir = "plots"
    transphotos_dir = "transparent_photos_front"
    transphotos_dir2 = "transparent_photos_inside"
    output_dir = "_final"
    Overlay_image_size = "400x300" 
    Overlay_top_location = "+1100+80" # 1000+50
    Overlay_bottom_location = "+1100+350" 
    
    SinglePlotMode_enabled = False #bool #Turns on root editor 
    SinglePlotMode_which_one = "3M Aura 9210+" 
    Output_print_level = 3
    ####################################################
    #set ouput_dir
    
    #############Load params from TOML#######################################
    #tomlData,ok = ut.tomlLoad(toml_config_file_path) 
    
    all_ok = True
    dp = ut.getDebugPrinter(tomlData)
    Overlay_image_size, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","image_size"], all_ok, default_val=Overlay_image_size)
    Overlay_top_location, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","top_location"], all_ok, default_val=Overlay_top_location)
    Overlay_bottom_location, _, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","bottom_location"], all_ok, default_val=Overlay_bottom_location)
    
    output_dir, ok, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","overlayDir"], all_ok, default_val=output_dir)
    output_dir = os.path.join('.', output_dir)
    
    transphotos_dir, ok, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","transphotos_dir_fronts"], all_ok, default_val=transphotos_dir)
    transphotos_dir = os.path.join('.', transphotos_dir)
    
    transphotos_dir2, ok, all_ok = ut.tomlGetSeq(tomlData, ["Overlay","transphotos_dir_insides"], all_ok, default_val=transphotos_dir2)
    transphotos_dir2 = os.path.join('.',transphotos_dir2)
    
    plots_dir, ok, all_ok = ut.tomlGetSeq(tomlData, ["Output","plotDir"], all_ok, default_val=plots_dir)
    plots_dir = os.path.join('.',plots_dir)
    
    SinglePlotMode_enabled, ok, all_ok = ut.tomlGetSeq(tomlData, ["SinglePlotMode","single_plot_mode_enabled"], all_ok, default_val=SinglePlotMode_enabled)
    SinglePlotMode_which_one , ok, all_ok = ut.tomlGetSeq(tomlData, ["SinglePlotMode","which_one"], all_ok, default_val=SinglePlotMode_which_one)
    ######################################################################################
    #Check that the directories exist.
    ut.assert_failExits(os.path.exists(plots_dir), f"Error! The plots directory does not exist {plots_dir}")
    ut.assert_failExits(os.path.exists(transphotos_dir), f"Error! The first transparent plots directory does not exist {transphotos_dir}")
    ut.assert_failExits(os.path.exists(transphotos_dir2),f"Error! The second transparent plots directory does not exist {transphotos_dir2}")
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
    #ls_result = os.popen(f"ls -b {plots_dir}/*.png").read().splitlines()
    plots      = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {plots_dir}/*.png")]
    transphotos = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir}/*.png")]
    transphotos2 = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir2}/*.png")]
    SinglePlotMode_which_one = backslashify_brackets(SinglePlotMode_which_one.strip(), backslashSpaces = True) + ".png"
    
    i = 0
    cnt_both= 0
    cnt_1st = 0
    cnt_2nd = 0
    for plot in plots:
        plot_stem = plot[len(plots_dir)+9:]
    
        if SinglePlotMode_enabled and SinglePlotMode_which_one != plot_stem:
            continue
        elif SinglePlotMode_enabled:
            print("found",SinglePlotMode_which_one)
    
        plotmorph = os.path.join(transphotos_dir, plot_stem) 
        plotmorph2 = os.path.join(transphotos_dir2, plot_stem) 
        plotmorphout = os.path.join(output_dir, plot[len(plots_dir)+1:] )
        #plotmorphout = output_dir + '/' + plot_stem 
        make_command_part1 = f"convert -quiet {plot} " #TODO add silencing, -quiet might not be right.
        make_command_part2 = f"\( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite "
        make_command_part3 = f"\( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite "
        make_command_part4 = f"{plotmorphout}" 
        #make_command = f"convert -quiet {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}"
    
        make_command = make_command_part1 + make_command_part2 + make_command_part4 
        #make_command = f"convert -quiet {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite {plotmorphout}" 
        #make_command = f"convert {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite {plotmorphout}" 
    
        make_command = make_command_part1 + make_command_part3 + make_command_part4 
        #make_command = f"convert {plot} \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}" 
        if plotmorph in transphotos and plotmorph2 in transphotos2:
            make_command = make_command_part1 + make_command_part2 + make_command_part3 + make_command_part4 
            #make_command = f"convert -quiet {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}"
            print("Making overlay",plotmorphout)
            #print(make_command)
            subprocess.run(make_command, shell=True) #os.system(make_command)
            cnt_both += 1
        elif plotmorph in transphotos:
            #make_command = f"convert -quiet {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite {plotmorphout}" 
            #make_command = f"convert {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite {plotmorphout}" 
            make_command = make_command_part1 + make_command_part2 + make_command_part4 
            print("Making overlay",plotmorphout)
            print(f"    no interior image for {plot_stem} found in {transphotos_dir2}")
            #print(make_command)
            subprocess.run(make_command, shell=True) #os.system(make_command)
            cnt_1st  += 1
        elif plotmorph2 in transphotos2:
            #make_command = f"convert -quiet {plot} \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}" 
            make_command = make_command_part1 + make_command_part3 + make_command_part4 
            #make_command = f"convert {plot} \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}" 
            print("Making overlay",plotmorphout)
            print(f"    no front image for {plot_stem} found in {transphotos_dir}")
            #print(make_command)
            subprocess.run(make_command, shell=True) #os.system(make_command)
            cnt_2nd += 1
        i += 1
    
    print(f"\nOverlay Process Report")
    print(f"    Out of {len(plots)} histograms in {plots_dir}")
    print(f"    {cnt_both} had matches for both overlays")
    print(f"    {cnt_1st} only had a match in {transphotos_dir}")
    print(f"    {cnt_2nd} only had a match in {transphotos_dir2}")
    print(f"    {len(plots) - cnt_both - cnt_1st - cnt_2nd} had no matches anywhere.")
    print(f"    {len(transphotos) - cnt_both - cnt_1st} unused files in {transphotos_dir}")
    print(f"    {len(transphotos2) -cnt_both - cnt_2nd} unused files in {transphotos_dir2}")

def make2Overlays_toml(toml_config_file_path:str) -> None:
    tomlData,ok = ut.tomlLoad(toml_config_file_path) 
    ut.assert_failExits(ok, f"Error! Failed to load toml config file {toml_config_file_path}")
    make2Overlays_tomldata(tomlData) 

if __name__ == "__main__":
    inlen = len(sys.argv)
    toml_config_file_path = "config.toml"
    if inlen > 1:
        toml_config_file_path = sys.argv[1]
    #too many inputs
    if inlen > 2:
        print(f"Warning! This takes at most 1 input. {inlen-1} inputs were specified")

    make2Overlays_toml(toml_config_file_path)
