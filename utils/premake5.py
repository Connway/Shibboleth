#!/usr/bin/env python

# Hopefully this script will become unnecessary in later version of Premake.
# This script is only applicable to Windows clients.

import utils.GenUtils
import subprocess
import platform
import sys
import re

# Main Application
using_physx = False
vs2015 = False
vs2017 = False

if platform.system() == "Windows":
	args = ["premake5"]
elif platform.system() == "Linux":
	args = ["premake5-linux"]
elif platform.system() == "Darwin":
	args = ["premake5-mac"]

args.extend(sys.argv)

for a in args:
	if re.match("premake5.py", a):
		args.remove(a)
		break

for a in args:
	if a == "--physx":
		using_physx = True
	elif a.lower() == "vs2015":
		vs2013 = True
	elif a.lower() == "vs2017":
		vs2015 = True

# proc_result = subprocess.run(args, stdout = subprocess.PIPE) # run premake and generate project files
# out_string = proc_result.stdout.decode("utf-8")

subprocess.run(args) # run premake and generate project files
print("")

#Fix VS2015 compiler warnings/errors
if using_physx:
	utils.GenUtils.FixPhysXProjects()
