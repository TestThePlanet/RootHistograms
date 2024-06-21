import os,sys
import subprocess
import urllib.request
import time
from inspect import getframeinfo
try:
    import toml
except ImportError:
    subprocess.check_call([sys.executable, "-m", "pip", "install", "toml"])
    import toml

#######################################################################################################################
#######################################################################################################################
def check_config_file_version(toml_config_file, config_file_version:int):
    #Check that the supplied config file has a version that is compatibe with this codebase
    required_min_config_file_version = 4

    if config_file_version < required_min_config_file_version:
        print(f"Error! Config file {toml_config_file} is version {config_file_version } which is requires at least version {required_min_config_file_version}")
        sys.exit()

#######################################################################################################################
class DebugPrinter:
    def __init__(self,print_level: int = 3):
        self.print_level = print_level 
    #-----------------------------------------------------------------------
    def print(self, thresh:int, *args) -> None:
        """
        Use: 
            import utils as ut
            from inspect import currentframe as here
            dp = ut.DebugPrinter(3)
            dp.print(3,"anything",55,"you",["want","to"])
        """
        if self.print_level >= thresh:
            if len(args) > 0:
                for item in args[:-1]:
                    print(item,end=' ')
                print(args[-1])
            else:
                print()
    #-----------------------------------------------------------------------
    def debug(self, currentFrame, thresh:int, *args) -> None:
        """
        Use: 
            import utils as ut
            from inspect import currentframe as here
            dp = ut.DebugPrinter(3)
            dp.debug(here(), 3, "hello","world",42)
        That prints:
            /mnt/d/abarker/Documents_Local/code/test.py line 10:
            hello world 42
        """
        if self.print_level >= thresh:
            frameinfo = getframeinfo(currentFrame)
            if len(args) > 0:
                print(f"{frameinfo.filename} line {frameinfo.lineno}:")
                for item in args[:-1]:
                    print(item,end=' ')
                print(args[-1])
            else:
                print(f"{frameinfo.filename} line {frameinfo.lineno}")

#######################################################################################################################
def getDebugPrinter(tomlData):
    print_level, ok, _  = tomlGetSeq(tomlData, ["Output","print_level"], True, default_val=3)
    if(not ok and print_level > 1):
        tomlConfigFileName = tomlData["tomlConfigFileName"]
        print(f"Error Unable to fetch print level from config file {tomlConfigFileName}")
    return DebugPrinter(print_level)

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
    except toml.TomlDecodeError:
        if exit_on_fail:
            print(f"Error! Unable to decode the TOML config file {toml_config_file}. Exiting!")
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
        data, ok = tomlLoad(toml_config_file, exit_on_fail=False)
        all_ok &= ok
        if ok:
            data_list.append(data)
    return data_list, all_ok

#######################################################################################################################
#######################################################################################################################
def assert_failPrints(ok:bool, message:str)->bool:
    if not ok:
        print(message)
    return ok

#######################################################################################################################
def assert_failExits(ok:bool, message:str)->None:
    if not ok:
        print(message)
        sys.exit()

#######################################################################################################################
def ensure_dir(directory_path: str, error_msg = "")->None:
    """
    Makes sure the directory exists. If not creates it. If it can't creat it, crash.
    """
    if error_msg == "":
        error_msg = f"Error, Could not find or create directory {directory_path}"
    ok = True
    try:
        if not os.path.exists(directory_path):
            os.makedirs(directory_path)
            print(f"    Info: Directory {directory_path} not found. Creating it.")
    except Exception as e:
        print(f"Error creating directory: {e}")
        ok = False
    assert_failExits(ok, error_msg)

#######################################################################################################################

def is_online() -> bool:
    try:
        # Ping Google's DNS server (8.8.8.8) with a single packet
        subprocess.check_output(["ping", "-c", "1", "8.8.8.8"])
        return True
    except subprocess.CalledProcessError:
        return False

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

    GoogleSheet_redownload , _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","redownload"], all_ok, default_val=True)
    GoogleSheet_CSV_URL, _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","CSV_URL"], all_ok, default_val=GoogleSheet_CSV_URL)
    output_TSV_file_name, _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","output_TSV_file_name"], all_ok, default_val="Main.tsv")
    download_timeout_sec, _, all_ok = tomlGetSeq(toml_data, ["GoogleSheet","download_timeout_sec"], all_ok, default_val=120)
    
    #Clear any existing error flags
    if os.path.exists(error_flag_file):
        print("Clearing previous error flag")
        os.remove(error_flag_file) #os.system(f"rm {error_flag_file}")

    if not GoogleSheet_redownload:
        print("Not downloading the google sheet because toml config has redownload = true.")
        return True
    
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
            print("    download completed")
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


