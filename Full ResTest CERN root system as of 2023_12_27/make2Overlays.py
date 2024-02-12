#!/usr/bin/env python   
import os,sys,string
"""
To execute

./makeOverlays.py [output_dir] [transparent_photos] [transparent_photos2] [plots_dir]

All directory inputs are optional. You can just put makeOverlays.py in the directory above plots/ and run it with no inputs.

./makeOverlays.py 

See default directories below. 
If plots and transphotos directories must exist, terminate early. 
If the output directory doesn't exist, it is created.
"""

####################################################

#Default Directories
plots_dir = "./plots" 
transphotos_dir = "./transparent_photos_front"
transphotos_dir2 = "./transparent_photos_inside"
output_dir = "./_final"

#Parameters for the convert commands
overlay_size = "500x300" # 
top_overlay_location = "+1100+50" # 1000+50
bottom_overlay_location = "+1100+350" 

####################################################

inlen = len(sys.argv)
if inlen > 1:
    output_dir = sys.argv[1]
if inlen > 2:
    transphotos_dir = sys.argv[2]
if inlen > 3:
    transphotos_dir2 = sys.argv[3]
if inlen > 4: 
    plots_dir = sys.argv[4]
if inlen > 5:
    print(f"Warning! This takes at most 5 inputs. {inlen-1} inputs were specified")

if not os.path.exists(plots_dir):
    print(f"Error! The plots directory does not exist {plots_dir}")
    sys.exit()
if not os.path.exists(transphotos_dir):
    print(f"Error! The first transparent plots directory does not exist {transphotos_dir}")
    sys.exit()
if not os.path.exists(transphotos_dir2):
    print(f"Error! The second transparent plots directory does not exist {transphotos_dir2}")
    sys.exit()
if not os.path.exists(output_dir):
    print(f"Output directory {output_dir} not found, creating it.")
    os.makedirs(output_dir)

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
    plotmorph = transphotos_dir+'/'+plot_stem 
    plotmorph2 = transphotos_dir2+'/'+plot_stem 
    plotmorphout = output_dir + '/' + plot_stem 
    if plotmorph in transphotos and plotmorph2 in transphotos2:
        make_command = f"convert {plot} \( {plotmorph} -resize {overlay_size} \) -geometry {top_overlay_location} -compose over -composite \( {plotmorph2} -resize {overlay_size} \) -geometry {bottom_overlay_location} -compose over -composite {plotmorphout}"
        #print(make_command)
        print("Making overlay",plotmorphout)
        os.system(make_command)
        cnt_both += 1
    elif plotmorph in transphotos:
        make_command = f"convert {plot} \( {plotmorph} -resize {overlay_size} \) -geometry {top_overlay_location} -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir2}")
        print("Making overlay",plotmorphout)
        #print(make_command)
        os.system(make_command)
        cnt_1st  += 1
    elif plotmorph2 in transphotos2:
        make_command = f"convert {plot} \( {plotmorph2} -resize {overlay_size} \) -geometry {bottom_overlay_location} -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir}")
        print("Making overlay",plotmorphout)
        #print(make_command)
        os.system(make_command)
        cnt_2nd += 1
    i += 1

print(f"Out of {len(plots)} histograms in {plots_dir}")
print(f"{cnt_both} had matches for both overlays")
print(f"{cnt_1st} only had a match in {transphotos_dir}")
print(f"{cnt_2nd} only had a match in {transphotos_dir2}")
print(f"{len(plots) - cnt_both - cnt_1st - cnt_2nd} had no matches anywhere.")
print(f"{len(transphotos) - cnt_both - cnt_1st} unused files in {transphotos_dir}")
print(f"{len(transphotos2) -cnt_both - cnt_2nd} unused files in {transphotos_dir2}")
