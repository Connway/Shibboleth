import sys
import re
import os

def ReadCacheOpenForWrite(file):
	f = open(file, "r")

	if not f:
		print("Failed to open '" + file + "' for READ.")
		exit()

	cache = f.read().split("\n")
	f.close()

	f = open(file, "w")

	if not f:
		print("Failed to open '" + file + "' for WRITE.")
		exit()

	return f, cache

def FixBrofilerProject(proj):
	brofiler_project, vcxproj_cache = ReadCacheOpenForWrite(proj)

	for line in vcxproj_cache:
		if re.match(".*HookFunction32.asm", line):
			brofiler_project.write("    <MASM Include=\"..\\..\\..\\dependencies\\brofiler\\src\\HookFunction32.asm\">\n")
			brofiler_project.write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">true</ExcludedFromBuild>\n")
			brofiler_project.write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">true</ExcludedFromBuild>\n")
			brofiler_project.write("    </MASM>\n")

		elif re.match(".*HookFunction64.asm", line):
			brofiler_project.write("    <MASM Include=\"..\\..\\..\\dependencies\\brofiler\\src\\HookFunction64.asm\">\n")
			brofiler_project.write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">true</ExcludedFromBuild>\n")
			brofiler_project.write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">true</ExcludedFromBuild>\n")
			brofiler_project.write("    </MASM>\n")

		elif re.match(".*ExtensionSettings", line):
			brofiler_project.write(line + "\n")
			brofiler_project.write("    <Import Project=\"$(VCTargetsPath)\\BuildCustomizations\\masm.props\" />\n");

		elif re.match(".*ExtensionTargets", line):
			brofiler_project.write(line + "\n")
			brofiler_project.write("    <Import Project=\"$(VCTargetsPath)\\BuildCustomizations\\masm.targets\" />\n");

		else:
			brofiler_project.write(line + "\n")

	brofiler_project.close()

def FixBrofilerFilters(proj):
	filter_file, filter_cache = ReadCacheOpenForWrite(proj + ".filters")

	for line in filter_cache:
		if re.match(".*HookFunction32.asm", line):
			filter_file.write("    <MASM Include=\"..\\..\\..\\dependencies\\brofiler\\src\\HookFunction32.asm\">\n")
		elif re.match(".*HookFunction64.asm", line):
			filter_file.write("    <MASM Include=\"..\\..\\..\\dependencies\\brofiler\\src\\HookFunction64.asm\">\n")
		elif line == "    </None>":
			filter_file.write("    </MASM>")
		else:
			filter_file.write(line + "\n")

	filter_file.close()

def FixPhysXProjectsHelper(dir, is_vs2015):
	for file in os.listdir(dir):
		if file.endswith(".vcxproj"):
			proj_file, proj_cache = ReadCacheOpenForWrite(dir + "/" + file)

			for line in proj_cache:
				index = line.find("/Fd$(TargetDir)\\$(TargetName).pdb")

				if index > -1:
					beg = line[:index]
					mid = "/Fd\"$(TargetDir)\\$(TargetName).pdb\""
					end = line[index + 33:]

					proj_file.write(beg + mid + end + "\n")

				elif is_vs2015 and line == "			<AdditionalOptions>/GR- /GF /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4577 /wd4774 /Zi /d2Zi+</AdditionalOptions>":
					proj_file.write("			<AdditionalOptions>/GR- /GF /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4577 /wd4774 /Zi /d2Zi+ /wd4464</AdditionalOptions>\n")
				elif is_vs2015 and line == "			<AdditionalOptions>/GR- /GF /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4577 /wd4774 /d2Zi+</AdditionalOptions>":
					proj_file.write("			<AdditionalOptions>/GR- /GF /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4577 /wd4774 /d2Zi+ /wd4464</AdditionalOptions>\n")
				else:
					proj_file.write(line + "\n")

def FixPhysXProjects():
	print("Fixing VS2013/VS2015 PhysX project files...")
	FixPhysXProjectsHelper("dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32", False)
	FixPhysXProjectsHelper("dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64", False)
	FixPhysXProjectsHelper("dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32", True)
	FixPhysXProjectsHelper("dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64", True)
