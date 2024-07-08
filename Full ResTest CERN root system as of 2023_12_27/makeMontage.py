#!/usr/bin/env python
import os,sys
import subprocess
import string
import time
import utils as ut

def makeMontage(tomlData) -> None:
    #############################################################################################
    #Hard coded constants:
    blacklist = ["FAKE-3M 9332+ LOT C203561.png","3M 8955J-DS2-01.png"]
    #TODO connect this correctly

    #############################################################################################
    #Hard coded backup default values:
    width = 10
    height = -1 #negative or 0 will use all images and 
    max_height = 22
    tile_sizex = 1024
    tile_sizey = 768
    picDir = "/mnt/d/abarker/Documents_Local/code/PlotMedia/Tests_hm_ordered_tester_7e66ab_and_Tester_67pfar" 
    output_file_name = "montage.png"
    output_file_name_GDrive = "montage.png"
    usePlus0 = False
    eogAtEnd = True
    subsampled_filename = "montage1024.png"
    make_subsample = True
    subsample_xres = 1024
    
    ############# Load params from TOML #######################################
    all_ok = True
    
    width , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","width"], all_ok, default_val=width )
    height , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","height"], all_ok, default_val=height )
    max_height , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","max_height"], all_ok, default_val=max_height )
    tile_sizex , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","tile_sizex"], all_ok, default_val=tile_sizex )
    tile_sizey , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","tile_sizey"], all_ok, default_val=tile_sizey )
    picDir , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","picDir"], all_ok, default_val=picDir )
    copy_to_google_drive, _, all_ok = ut.tomlGetSeq(tomlData, ["ProcessCtrl","copy_to_google_drive"], all_ok, default_val=False )
    output_file_name , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","output_file_name"], all_ok, default_val=output_file_name )
    output_file_name_GDrive , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","output_file_name_GDrive"], all_ok, default_val=output_file_name )
    usePlus0 , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","usePlus0"], all_ok, default_val=usePlus0 )
    eogAtEnd , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","eogAtEnd"], all_ok, default_val=eogAtEnd )
    #blacklist , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","blacklist"], all_ok, default_val=blacklist) #TODO connect this correctly
    
    subsampled_filename , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","subsampled_filename"], all_ok, default_val=subsampled_filename )
    make_subsample , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","make_subsample"], all_ok, default_val=make_subsample )
    subsample_xres , _, all_ok = ut.tomlGetSeq(tomlData, ["Montage","subsample_xres"], all_ok, default_val=subsample_xres )
    
    
    ############# Validate params from TOML #######################################
    ut.ensure_dir(picDir)
    ut.assert_failPrints(all_ok, f"Warning! Unable to read some paramters from the TOML file. Resorting to hard-coded backups")
    
    ##################################################################################################
    
    file_list = os.listdir(picDir)
    file_list = [file for file in file_list if file.endswith(".png")]
    file_list.reverse()
    
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
        if usePlus0:
            sub_run_header = ["montage", "-background", "none", "-tile",f"{width}x{heights[i]}","-geometry", f"{tile_sizex}x{tile_sizey}>+0+0"]
        else:
            sub_run_header = ["montage", "-background", "none", "-tile",f"{width}x{heights[i]}","-geometry", f"{tile_sizex}x{tile_sizey}"]
        sub_run_suffix = [f"tempMontage{i}.png",]
        stage2list += sub_run_suffix 
        sub_run_list =  sub_run_header + truncated_filepath_list[i*width*4:(i+1)*width*4] + sub_run_suffix 
        print(f"making strip {i+1} of {n}. This may take a minuit")
        st = time.time()
        #print(sub_run_list)
        subprocess.run(sub_run_list)
        et = time.time()
        print(f"    strip {i} completed after {et-st:.2f} seconds." )
    
    ## stage 2
    sub_run_header = ["montage", "-background", "none", "-tile",f"1x{n}","-geometry", "+0+0"]
    sub_run_list =  sub_run_header + stage2list + [f"{output_file_name}",]
    print(f"Making the main montage. Sit tight, this takes a couple minutes.")
    st = time.time()
    #print(sub_run_list)
    subprocess.run(sub_run_list)
    et = time.time()
    print(f"Created {output_file_name}")
    print(f"    Main montage completed after {et-st:.2f} seconds." )
    
    ## Subsample
    if make_subsample:
        sub_run_list = ["convert", f"{output_file_name}", "-resize", f"{subsample_xres}x", subsampled_filename ]
        print(f"Making the subsampled montage. {subsampled_filename}")
        st = time.time()
        #convert montage.png -resize 1024x montage1024.png
        subprocess.run(sub_run_list)
        et = time.time()
        print(f"Created {subsampled_filename}")
        print(f"    Subsampled montage completed after {et-st:.2f} seconds." )
    
    #delete intermediate temp files
    for temp in stage2list:
        os.remove(temp)
    
    if copy_to_google_drive:
        print("copying to {output_file_name} google drive")
        subprocess.run(["cp", output_file_name, output_file_name_GDrive])
        print("    done")
    
    if eogAtEnd:
        if make_subsample:
            subprocess.run(["eog",f"{subsampled_filename}"])
        else:
            subprocess.run(["eog",f"{output_file_name}"])
    
    #montage  -background none  -tile 2x2 -geometry +0+0 3M\ 1860.png 3M\ 8210.png 3M\ 9132.png 3M\ AFFM.png  output.png
    #montage *.png -tile 10x19 -geometry +0+0 -border 2 -bordercolor black output.png

def makeMontage_fromConfigFileName(toml_config_file_path:str) -> None:
    tomlData,ok = ut.tomlLoad(toml_config_file_path)
    ut.assert_failExits(ok, f"Error! Failed to load toml config file {toml_config_file_path}")
    makeMontage(tomlData)

if __name__ == "__main__":
    inlen = len(sys.argv)
    toml_config_file_path = "config.toml"
    #toml_config_file_path = "config_montage.toml"
    if inlen > 1:
        toml_config_file_path = sys.argv[1]
    if inlen > 2: #too many inputs
        print(f"Warning! This takes at most 1 input. {inlen-1} inputs were specified")

    makeMontage_fromConfigFileName(toml_config_file_path)
