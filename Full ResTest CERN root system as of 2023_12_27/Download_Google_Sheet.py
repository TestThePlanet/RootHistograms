#!/usr/bin/python3
import utils as ut

ut.Download_Google_Sheet( ut.tomlLoad("config.toml", exit_on_fail= True)[0] )
