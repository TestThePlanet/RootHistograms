#!/usr/bin/python3
import utils as ut
data, all_ok = ut.tomlLoad(".config.toml", exit_on_fail = True)
print(all_ok)
