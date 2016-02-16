import sys
import re

def FixBrofilerProject(proj):
	brofiler_project = open(proj, "r")

	if not brofiler_project:
		print("Failed to open '" + proj + "' for READ.")
		exit()

	vcxproj_cache = brofiler_project.read().split("\n")
	brofiler_project.close()

	brofiler_project = open(proj, "w")

	if not brofiler_project:
		print("Failed to open '" + proj + "' for WRITE.")
		exit()

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
	filter_file = open(proj + ".filters", "r")

	if not filter_file:
		print("Failed to open '" + proj + ".filters" + "' for READ.")
		exit()

	filter_cache = filter_file.read().split("\n")
	filter_file.close()

	filter_file = open(proj + ".filters", "w")

	if not filter_file:
		print("Failed to open '" + proj + ".filters" + "' for WRITE.")
		exit()

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
