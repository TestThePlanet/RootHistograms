#!/usr/bin/env python   
import os,sys
import subprocess
import string
import utils as ut

"""
To execute

./make2Overlays.py [output_dir] [transparent_photos] [transparent_photos2] [plots_dir]

All directory inputs are optional. You can just put makeOverlays.py in the directory above plots/ and run it with no inputs.
If command line arguments are not supplied, values are drawn from the TOML config file. 
If that's not there, hard coded defaults are used. 

./make2Overlays.py 

See default directories below. 
If plots and transphotos directories must exist, terminate early. 
If the output directory doesn't exist, it is created.
"""

####################################################
data, ok = ut.tomlLoad("config.toml")

#Hard coded backup default values:
plots_dir = "plots"
transphotos_dir = "transparent_photos_front"
transphotos_dir2 = "transparent_photos_inside"
output_dir = "_final"
Overlay_image_size = "400x300" 
Overlay_top_location = "+1100+80" # 1000+50
Overlay_bottom_location = "+1100+350" 

####################################################

#Load params from TOML
all_ok = True
Overlay_image_size,     _, all_ok = ut.tomlGetSeq(data, ["Overlay","image_size"], all_ok, default_val=Overlay_image_size)
Overlay_top_location,   _, all_ok = ut.tomlGetSeq(data, ["Overlay","top_overlay_location"], all_ok, default_val=Overlay_top_location)
Overlay_bottom_location,_, all_ok = ut.tomlGetSeq(data, ["Overlay","bottom_overlay_location"], all_ok, default_val=Overlay_bottom_location)

inlen = len(sys.argv)
#set ouput_dir
if inlen > 1:
    output_dir = sys.argv[1]
else:
    output_dir, ok, all_ok = ut.tomlGetSeq(data, ["Overlay","overlayDir"], all_ok, default_val=output_dir)

output_dir = os.path.join('.',output_dir)

#set transphotos_dir
if inlen > 2:
    transphotos_dir = sys.argv[2]
else:
    transphotos_dir, ok, all_ok  = ut.tomlGetSeq(data, ["Overlay","transphotos_dir_fronts"], all_ok, default_val=transphotos_dir)

transphotos_dir = os.path.join('.', transphotos_dir)

#set transphotos_dir2
if inlen > 3:
    transphotos_dir2 = sys.argv[3]
else:
    transphotos_dir2, ok, all_ok = ut.tomlGetSeq(data, ["Overlay","transphotos_dir_insides"], all_ok, default_val=transphotos_dir2)

transphotos_dir2 = os.path.join('.',transphotos_dir2)

#set plots dir
if inlen > 4: 
    plots_dir = sys.argv[4]
else:
    plots_dir, ok, all_ok  = ut.tomlGetSeq(data, ["Output","plotDir"], all_ok, default_val=plots_dir)
plots_dir = os.path.join('.',plots_dir)

#too many inputs
if inlen > 5:
    print(f"Warning! This takes at most 5 inputs. {inlen-1} inputs were specified")

ut.assert_failExits(os.path.exists(plots_dir), f"Error! The plots directory does not exist {plots_dir}")
ut.assert_failExits(os.path.exists(transphotos_dir), f"Error! The first transparent plots directory does not exist {transphotos_dir}")
ut.assert_failExits(os.path.exists(transphotos_dir2,f"Error! The second transparent plots directory does not exist {transphotos_dir2}")
ut.assert_failExits(os.path.exists(output_dir),f"Output directory {output_dir} not found, creating it.")

ut.assert_failPrints(all_ok, f"Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")

def backslashify_brackets(fname): #str -> str
    fname = fname.replace(')', '\)')
    fname = fname.replace('(', '\(')
    fname = fname.replace('[', '\[')
    fname = fname.replace(']', '\]')
    fname = fname.replace('{', '\{')
    fname = fname.replace('}', '\}')
    return fname 

#Read in lists of images in the plots and transphotos dirs
#ls_result = os.popen(f"ls -b {plots_dir}/*.png").read().splitlines()
#plots      = [backslashify_brackets(plot.strip()) for plot in ls_result]
plots      = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {plots_dir}/*.png")]
#plot_stems  = [backslashify_brackets(plot.strip()[8:]) for plot in ls_result]
transphotos = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir}/*.png")]
transphotos2 = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir2}/*.png")]

i = 0
cnt_both= 0
cnt_1st = 0
cnt_2nd = 0
for plot in plots:
    plot_stem = plot[len(plots_dir)+9:]
    plotmorph = os.path.join(transphotos_dir, plot_stem) 
    plotmorph2 = os.path.join(transphotos_dir2, plot_stem) 
    plotmorphout = os.path.join(output_dir, plot[len(plots_dir)+1:] )
    #plotmorphout = output_dir + '/' + plot_stem 
    if plotmorph in transphotos and plotmorph2 in transphotos2:
        make_command = f"convert {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}"
        print("Making overlay",plotmorphout)
        print(make_command)
        os.system(make_command)
        cnt_both += 1
    elif plotmorph in transphotos:
        make_command = f"convert {plot} \( {plotmorph} -resize {Overlay_image_size} \) -geometry {Overlay_top_location} -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir2}")
        print("Making overlay",plotmorphout)
        print(make_command)
        os.system(make_command)
        cnt_1st  += 1
    elif plotmorph2 in transphotos2:
        make_command = f"convert {plot} \( {plotmorph2} -resize {Overlay_image_size} \) -geometry {Overlay_bottom_location} -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir}")
        print("Making overlay",plotmorphout)
        print(make_command)
        os.system(make_command)
        cnt_2nd += 1
    i += 1

print(f"\nOut of {len(plots)} histograms in {plots_dir}")
print(f"{cnt_both} had matches for both overlays")
print(f"{cnt_1st} only had a match in {transphotos_dir}")
print(f"{cnt_2nd} only had a match in {transphotos_dir2}")
print(f"{len(plots) - cnt_both - cnt_1st - cnt_2nd} had no matches anywhere.")
print(f"{len(transphotos) - cnt_both - cnt_1st} unused files in {transphotos_dir}")
print(f"{len(transphotos2) -cnt_both - cnt_2nd} unused files in {transphotos_dir2}")
