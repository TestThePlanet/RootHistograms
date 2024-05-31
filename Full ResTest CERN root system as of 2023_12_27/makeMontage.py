#!/usr/bin/env python
import os,sys
import subprocess
import string
import time
#import utils as ut

width = 10
height = -1 #negative or 0 will use all images and 
max_height = 50
tile_sizex = 1024
tile_sizey = 768
picDir = "transparent_photos_front"
output_file_name = "montage.png"

#blacklist = ["Funight Vertical Bifold Earloop KN95.png",]
blacklist = ["FAKE-3M 9332+ LOT C203561.png","3M 8955J-DS2-01.png"]
##################################################################################################
file_list = os.listdir(picDir)
file_list = [file for file in file_list if file.endswith(".png")]

#remove blacklisted files
for bl in blacklist:
    if bl in file_list:
        file_list.remove(bl)

n_files = len(file_list)
if height <= 0:
    height = min(n_files // width,max_height) #truncating division
print(f"Constructing montage of {width} wide x {height} heigh")

truncated_filepath_list = [picDir+'/'+file for file in file_list[:width*height]]

#setup for making strips
n_reg = height // 4
h_extra = height % 4
n = n_reg + min(h_extra,1)
heights = [4,]*n_reg
if h_extra > 0:
    heights.append(h_extra)
sst = time.time()
stage2list = []

for i in range(n):
    sub_run_header = ["montage", "-background", "none", "-tile",f"{width}x{heights[i]}","-geometry", f"{tile_sizex}x{tile_sizey}>+0+0"]
    sub_run_suffix = [f"tempMontage{i}.png",]
    stage2list += sub_run_suffix 
    sub_run_list =  sub_run_header + truncated_filepath_list[i*width*4:(i+1)*width*4] + sub_run_suffix 
    print(f"making strip {i+1} of {n}. This may take a minuit")
    st = time.time()
    subprocess.run(sub_run_list)
    et = time.time()
    print(f"    strip {i} completed after {et-st:.2f} seconds." )

## stage 2
sub_run_header = ["montage", "-background", "none", "-tile",f"1x{n}","-geometry", "+0+0"]
sub_run_list =  sub_run_header + stage2list + [f"{output_file_name}",]
print(f"Making the main montage. Sit tight, this takes a couple minutes.")
st = time.time()
subprocess.run(sub_run_list)
et = time.time()

#delete intermediate temp files
for temp in stage2list:
    os.remove(temp)

print(f"    Main montage completed after {et-st:.2f} seconds." )
print(f"    Total time: {et-sst:.2f} seconds to process." )
print(f"Created {output_file_name}")

subprocess.run(["eog",f"{output_file_name}"])

#montage  -background none  -tile 2x2 -geometry +0+0 3M\ 1860.png 3M\ 8210.png 3M\ 9132.png 3M\ AFFM.png  output.png
#montage *.png -tile 10x19 -geometry +0+0 -border 2 -bordercolor black output.png
