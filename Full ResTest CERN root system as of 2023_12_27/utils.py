import os,sys
import subprocess
import urllib.request
import time

try:
    import toml
except ImportError:
    subprocess.check_call([sys.executable, "-m", "pip", "install", "toml"])
    import toml

#######################################################################################################################

def tomlGet(tomlLoadData, key:str, all_ok = None, default_val = -1):
    #single key
    #tomlLoadData is a nested dictionary, default_val doesn't have to be a int and should match the expected value type.
    #If the key is valid, returns the toml data at that key and true. else return default value and false.
    #If all_ok {bool or None} is given, there is a 3rd output = all_ok & ok. Use for chains of these.
    #returns value, value_was_found_successfully_and_not_default, [all_ok]
    key = key.strip()
    if key in tomlLoadData:
        if all_ok == None:
            return tomlLoadData[key], True
        else:
            return tomlLoadData[key], True, all_ok
    else:
        print(f"Warning: failed to fetch {key} from TOML file")
        if all_ok == None:
            return default_val, False
        else:
            return default_val, False, False

#######################################################################################################################

def tomlGetSeq(tomlLoadData, key_list, all_ok = None, default_val = -1):
    #list of keys, otherwise just like tomlGet
    if len(key_list) <= 0:
        print("Error Unable to tomlGet an empty key list")
        return default_val, False
    elif len(key_list) == 1:
        return tomlGet(tomlLoadData, key_list[0], all_ok, default_val)
    else: # len > 1
        if key_list[0] in tomlLoadData:
            return tomlGetSeq(tomlLoadData[key_list[0]], key_list[1:], all_ok, default_val)
        else: #key not found
            if all_ok == None:
                return default_val, False
            else:
                return default_val, False, False

#######################################################################################################################

def tomlLoad(toml_config_file:str, exit_on_fail:bool = True):
    #Loads the toml config file
    #Takes in the config file path, and a bool exit_on_failure telling whether to crash if there's a problem loading
    #Returns the toml config as a nested dictionary, and a bool = True if it's ok.
    #The name of the toml config file is appended to the top-level dictionary under the tag "tomlConfigFileName"

    if not os.path.exists(toml_config_file):
        if exit_on_fail:
            print(f"Error! Unable to find the TOML config file {toml_config_file}, no such file. Exiting!" )
            sys.exit()
        else:
            print(f"Warning! Unable to find the TOML config file {toml_config_file}, no such file." )
        return {},False

    try:
        data = toml.load(toml_config_file)
        data["tomlConfigFileName"] = toml_config_file 
        return data, True
    except TypeError:
        if exit_on_fail:
            print("Error! Type Error for TOML config file {toml_config_file}: non-string passed. Exiting!")
            sys.exit() 
        else:
            print("Warning! Type Error for TOML config file {toml_config_file}: non-string passed.")
    except TomlDecodeError:
        if exit_on_fail:
            print("Error! Unable to decode the TOML config file {toml_config_file}. Exiting!")
            sys.exit() 
        else:
            print("Warning! Unable to decode the TOML config file {toml_config_file}.")
    return {},False

#######################################################################################################################

def tomlLoadAll(list_of_config_file_names):
    #takes a list of config file names
    #returns a list of dictionaries corresponding to those toml 
    data_list = []
    all_ok = True
    for toml_config_file in list_of_config_file_names:
        data, ok = tomlLoad(toml_config_file, exit_on_fail=False):
        all_ok &= ok
        if ok:
            data_list.append(data)
    return data_list, all_ok

#######################################################################################################################
#######################################################################################################################

def assert_failPrints(ok, message):
    if not ok:
        print(message)

def assert_failExits(ok, message):
    if not ok:
        print(message)
        sys.exit()

#######################################################################################################################
#######################################################################################################################

def Download_Google_Sheet(toml_data):
    #Takes a nested dictionary that originates from a toml config file. 
    #returns true iff download went ok

    #Hard coded constants
    temp_CSV_downloaded_file = "data.csv"
    error_flag_file = "error.flag"

    #TOML imputs
    all_ok = True
    GoogleSheet_CSV_URL = "https://docs.google.com/spreadsheets/d/1arv8PObW_O4HMpScei3KAlt0zj1C55v_/gviz/tq?tqx=out:csv&sheet=Main"
    GoogleSheet_CSV_URL, _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","CSV_URL"], all_ok, default_val=GoogleSheet_CSV_URL)
    output_TSV_file_name, _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","output_TSV_file_name"], all_ok, default_val="Main.tsv")
    download_timeout_sec, _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","download_timeout_sec"], all_ok, default_val=120)
    
    #Clear any existing error flags
    if os.path.exists(error_flag_file):
        print("Clearing previous error flag")
        os.remove(error_flag_file) #os.system(f"rm {error_flag_file}")
    
    #Download the file
    print("Downloading the Google Sheet")
    urllib.request.urlretrieve( GoogleSheet_CSV_URL, temp_CSV_downloaded_file )
    
    #Guard against continuing after a failed download.
    download_went_ok = False
    if os.path.exists(temp_CSV_downloaded_file):
        file_time = os.path.getmtime(temp_CSV_downloaded_file) # Get the time of the last modification of the file #also getctime
        current_time = time.time() # Get the current time
        if current_time - file_time <= download_timeout_sec: # Check if the file's timestamp is within the last 3 minutes 
            download_went_ok = True
            print("Download completed")
        else:
            print(f'Error! The Google Sheet was not downloaded. Curernt {temp_CSV_downloaded_file} file is out of date.')
    else:
        print(f'Error! The Google Sheet was not downloaded. No {temp_CSV_downloaded_file} file was found.')
    
    if download_went_ok:
        sedcommand = f"""sed 's/","/\t/g; s/^"//; s/"$//; s/""/"/g' {temp_CSV_downloaded_file} > {output_TSV_file_name}"""
        #print(sedcommand) #DEBUG
        subprocess.run(sedcommand, shell=True) #os.system(sedcommand)
    
        os.remove(temp_CSV_downloaded_file) if os.path.exists(temp_CSV_downloaded_file) else None
        return all_ok
    else:
        #Set error flag to inhibit root from running
        #os.system(f"echo '1' > {error_flag_file}")
        with open(error_flag_file, 'w') as file:
            file.write('1')
        return False

