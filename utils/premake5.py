#!/usr/bin/env python

# Hopefully this script will become unnecessary in later version of Premake.
# This script is only applicable to Windows clients.

import utils.GenUtils
import subprocess
import platform
import sys
import re

# Main Application
using_brofiler = False
using_physx = False
vs2013 = False
vs2015 = False

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
	if a == "--brofiler":
		using_brofiler = True
	elif a == "--physx":
		using_physx = True
	elif a.lower() == "vs2013":
		vs2013 = True
	elif a.lower() == "vs2015":
		vs2015 = True

# proc_result = subprocess.run(args, stdout = subprocess.PIPE) # run premake and generate project files
# out_string = proc_result.stdout.decode("utf-8")

subprocess.run(args) # run premake and generate project files
print("")

#Fix VS2015 compiler warnings/errors
if using_physx:
	utils.GenUtils.FixPhysXProjects()

# Modify the *.vcxproj file for assembly files
if using_brofiler:
	brofiler_project_path = ""

	if vs2013:
		brofiler_project_path = "project/vs2013/dependencies/brofiler.vcxproj"
	elif vs2015:
		brofiler_project_path = "project/vs2015/dependencies/brofiler.vcxproj"

	print("Modifying 'brofiler' project file...")
	utils.GenUtils.FixBrofilerProject(brofiler_project_path)

	print("Modifying 'brofiler' project filters file...")
	utils.GenUtils.FixBrofilerFilters(brofiler_project_path)