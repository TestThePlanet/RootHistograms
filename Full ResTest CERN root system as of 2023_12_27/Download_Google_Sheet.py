#!/usr/bin/python3
import urllib.request
import os
import time

Google_Sheet_URL_CSV = "https://docs.google.com/spreadsheets/d/1arv8PObW_O4HMpScei3KAlt0zj1C55v_/gviz/tq?tqx=out:csv&sheet=Main"
temp_CSV_downloaded_file = "data.csv"
output_TSV_file_name = "Main2.tsv"

error_flag_file = "error.flag"

#Clear any existing error flags
if os.path.exists(error_flag_file):
    print("Clearing previous error flag")
    os.system(f"rm {error_flag_file}")

#Download the file
print("Downloading the Google Sheet")
urllib.request.urlretrieve( Google_Sheet_URL_CSV, temp_CSV_downloaded_file )

#Guard against continuing after a failed download.
download_went_ok = False
if os.path.exists(temp_CSV_downloaded_file):
    file_time = os.path.getmtime(temp_CSV_downloaded_file) # Get the time of the last modification of the file #also getctime
    current_time = time.time() # Get the current time
    if current_time - file_time <= 180: # Check if the file's timestamp is within the last 3 minutes #TODO make that smaller
        download_went_ok = True
        print("Download completed")
    else:
        print(f'Error! The Google Sheet was not downloaded. Curernt {temp_CSV_downloaded_file} file is out of date.')
else:
    print(f'Error! The Google Sheet was not downloaded. No {temp_CSV_downloaded_file} file was found.')

if download_went_ok:
    sedcommand = f"""sed 's/","/\t/g; s/^"//; s/"$//; s/""/"/g' {temp_CSV_downloaded_file} > {output_TSV_file_name}"""
    #sed -i 's/","/\t/g' data.csv; sed -i 's/^"//' data.csv; sed -i 's/"$//' data.csv
    #print(sedcommand) #DEBUG
    os.system(sedcommand)

    rmcommand = f"rm {temp_CSV_downloaded_file}"
    os.system(rmcommand) #DEBUG reinstate
    #print(rmcommand) #DEBUG
else:
    #Set error flag to inhibit root from running
    os.system(f"echo '1' > {error_flag_file}")

#import pandas as pd
#df = pd.read_csv(temp_CSV_downloaded_file)
#df.to_csv(output_TSV_file_name, sep='\t', index=False)



