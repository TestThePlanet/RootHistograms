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

#Default Directories
plots_dir = "./plots" 
transphotos_dir = "./transparent_photos_front"
transphotos_dir2 = "./transparent_photos_inside"
output_dir = "./_final"

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
ls_result = os.popen(f"ls -b {plots_dir}/*.png")
plots      = [backslashify_brackets(plot.strip()) for plot in ls_result]
plot_stems  = [backslashify_brackets(plot.strip()[8:]) for plot in ls_result]
transphotos = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir}/*.png")]
transphotos2 = [backslashify_brackets(plot.strip()) for plot in os.popen(f"ls -b {transphotos_dir2}/*.png")]

for plot,plot_stem in zip(plots,plot_stems):
    plotmorph = plot_stem.replace(plots_dir, transphotos_dir)
    plotmorph2 = plot_stem.replace(plots_dir, transphotos_dir2)
    plotmorphout = plot.replace(plots_dir, output_dir)
    if plotmorph in transphotos and plotmorph2 in transphotos2:
        make_command = f"convert {plot} \( {plotmorph} -resize 700x500 \) -geometry +900+50 -compose over -composite \( {plotmorph2} -resize 700x500 \) -geometry +900+150 -compose over -composite {plotmorphout}" 
        print(make_command)
        #print(f"Making overlay:{plot}")
        os.system(make_command)
    elif plotmorph in transphotos:
        make_command = f"convert {plot} \( {plotmorph} -resize 700x500 \) -geometry +900+50 -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir2}")
        print(make_command)
        os.system(make_command)
    elif plotmorph2 in transphotos2:
        make_command = f"convert {plot} \( {plotmorph2} -resize 700x500 \) -geometry +900+150 -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir}")
        print(make_command)
        os.system(make_command)
print("end")
