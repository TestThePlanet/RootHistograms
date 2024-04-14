#!/usr/bin/env python   
import os,sys,
import subprocess
import string

try:
    import toml
except ImportError:
    subprocess.check_call([sys.executable, "-m", "pip", "install", "toml"])
    import toml

def tomlGet(tomlLoadData, key, default_val = -1):
    #single key
    #If the key is valid, returns the toml data at that key and true. else return default value and false.
    if key in tomlLoadData:
        return tomlLoadData[key], True
    else:
        print(f"Warning: failed to fetch {key} from TOML file")
        return default_val, False
def tomlGetSeq(tomlLoadData, key_list, default_val = -1):
    if len(key_list) == 1:
        return tomlGet(tomlLoadData, key_list[0], default_val)
    elif len(key_list) <= 0:
        print("Error Unable to tomlGet an empty key list")
        return default_val, False
    else:
        if key_list[0] in tomlLoadData:
            return tomlGetSeq(tomlLoadData[key_list[0]], key_list[1:], default_val)
        else:
            return default_val, False

"""
This version relies more hevily on the toml config file. 

To execute

./make2Overlays.py [config_file.toml]

The config file input are optional. You can just put makeOverlays.py in the directory above plots/ and run it with no inputs

./make2Overlays.py 

In that case, config.toml is assumed. 

See default directories below. 
If plots and transphotos directories must exist, terminate early. 
If the output directory doesn't exist, it is created.
"""

####################################################
#Hard coded backup default values:
toml_config_file = "config.toml"
plots_dir = "plots"
transphotos_dir = "transparent_photos_front"
transphotos_dir2 = "transparent_photos_inside"
output_dir = "_final"
overlay_size = "400x300" 
top_overlay_location = "+1100+80" # 1000+50
bottom_overlay_location = "+1100+350" 
####################################################
inlen = len(sys.argv)
#set ouput_dir
if inlen > 1:
    toml_config_file = sys.argv[1]

#too many inputs
if inlen > 2:
    print(f"Warning! This takes at most 1 input. {inlen-1} inputs were specified")
####################################################
#Load params from TOML
try:
    data = toml.load("config.toml") 
except TypeError:
    print("Type Error, non-string passed")
except TomlDecodeError:
    print("Error decoding the toml file")

all_ok = True
overlay_size, ok = tomlGetSeq(data, ["Overlay","overlayDir"], overlay_size)
all_ok &= not ok
top_overlay_location, ok = tomlGetSeq(data, ["Overlay","overlayDir"], top_overlay_location)
all_ok &= not ok
bottom_overlay_location , ok = tomlGetSeq(data, ["Overlay","overlayDir"], bottom_overlay_location)
all_ok &= not ok

#set output/overlays dir
output_dir, ok = tomlGetSeq(data, ["Overlay","overlayDir"], output_dir)
all_ok &= not ok
output_dir = "./" + output_dir 

#set transphotos_dir
transphotos_dir, ok = tomlGetSeq(data, ["Overlay","transphotos_dir_fronts"], transphotos_dir)
all_ok &= not ok
transphotos_dir = "./" + transphotos_dir

#set transphotos_dir2
transphotos_dir2, ok = tomlGetSeq(data, ["Overlay","transphotos_dir_insides"], transphotos_dir2)
all_ok &= not ok
transphotos_dir2 = "./" + transphotos_dir2 

#set plots dir
plots_dir, ok = tomlGetSeq(data, ["Output","plotDir"], plots_dir)
all_ok &= not ok
plots_dir = "./"+plots_dir

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
if not all_ok:
    print(f"Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")
    #sys.exit()

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
    plotmorphout = output_dir + '/' + plot[len(plots_dir)+1:]
    #plotmorphout = output_dir + '/' + plot_stem 
    make_cmd_list = ["convert",#0
            plot, 
            "\(",#2
            plotmorph, 
            "-resize",#4
            overlay_size, 
            "\)",#6 #TODO need backslashes??
            "-geometry", 
            top_overlay_location,#8
            "-compose", 
            "over",#10
            "-composite", 
            "\(",#12
            plotmorph2, 
            "-resize",#14
            overlay_size, 
            "\)",#16
            "-geometry", 
            bottom_overlay_location,#18
            "-compose", 
            "over",#20
            "-composite", 
            plotmorphout]
    if plotmorph in transphotos and plotmorph2 in transphotos2:
        print("Making overlay",plotmorphout)
        subprocess.run( make_cmd_list )
        cnt_both += 1
    elif plotmorph in transphotos:
        make_cmd_list = make_cmd_list[:12] + make_cmd_list[-1:]
        #make_command = f"convert {plot} \( {plotmorph} -resize {overlay_size} \) -geometry {top_overlay_location} -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir2}")
        print("Making overlay",plotmorphout)
        subprocess.run( make_cmd_list )
        #os.system(make_command)
        cnt_1st  += 1
    elif plotmorph2 in transphotos2:
        make_cmd_list = make_cmd_list[:2] + make_cmd_list[12:]
        #make_command = f"convert {plot} \( {plotmorph2} -resize {overlay_size} \) -geometry {bottom_overlay_location} -compose over -composite {plotmorphout}" 
        print(f"No plot for {plot_stem} found in {transphotos_dir}")
        print("Making overlay",plotmorphout)
        subprocess.run( make_cmd_list )
        #os.system(make_command)
        cnt_2nd += 1
    i += 1

print(f"\nOut of {len(plots)} histograms in {plots_dir}")
print(f"{cnt_both} had matches for both overlays")
print(f"{cnt_1st} only had a match in {transphotos_dir}")
print(f"{cnt_2nd} only had a match in {transphotos_dir2}")
print(f"{len(plots) - cnt_both - cnt_1st - cnt_2nd} had no matches anywhere.")
print(f"{len(transphotos) - cnt_both - cnt_1st} unused files in {transphotos_dir}")
print(f"{len(transphotos2) -cnt_both - cnt_2nd} unused files in {transphotos_dir2}")
