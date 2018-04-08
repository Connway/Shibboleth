# <ClCompile Include="..\..\src\common\dummy.cpp" />
# <ClCompile Include="..\..\src\common\dummy.cpp">

import sys
import re

def FindSourceFile(line):
	m = re.search(r'<ClCompile Include="(.+(?:\.cpp|\.c))" />', line)
	has_options = False

	if not m:
		m = re.search(r'<ClCompile Include="(.+(?:\.cpp|\.c))">', line)
		has_options = True

		if not m:
			return None

	file_path = m.group(1)[len("..\\..\\"):]
	return [file_path, has_options]

def FindIncludeFile(line):
	m = re.search(r'<ClInclude Include="(.+\.h)" />', line)

	if not m:
		return None

	file_path = m.group(1)[len("..\\..\\"):]
	return [file_path, False]

def IsExcludedFromBuild(line):
	index = line.find("ExcludedFromBuild")
	return index > -1

def IsPrecompiledHeader(line):
	index = line.find("PrecompiledHeader")
	return index > -1


if len(sys.argv) < 2:
	print("No project file specified.")
	exit()

try:
	file = open(sys.argv[1], "r")

except FileNotFoundError:
	print("Failed to open file '" + sys.argv[1] + "'.")
	exit()

sources = []
includes = []
file_path = None

for line in file:
	if file_path:
		if IsExcludedFromBuild(line):
			file_path = None
			continue
		# elif IsPrecompiledHeader(line):
		# 	file_path = None
		# 	continue
		else:
			sources.append(file_path[0])
			file_path = None
			continue

	file_path = FindSourceFile(line)
	is_source = True

	if not file_path:
		file_path = FindIncludeFile(line)
		is_source = False

	if not file_path:
		continue

	if file_path[1]:
		continue

	if is_source:
		sources.append(file_path[0])
	else:
		includes.append(file_path[0])

	file_path = None

file.close()

final_str = "files\n{"
first = True

for f in includes:
	if not first:
		final_str = final_str + ","

	final_str = final_str + "\n\t\"" + f.replace("\\", "/") + "\""
	first = False

for f in sources:
	if not first:
		final_str = final_str + ","

	final_str = final_str + "\n\t\"" + f.replace("\\", "/") + "\""
	first = False

final_str = final_str + "\n}\n"

if len(sys.argv) < 3:
	print(final_str)
	exit()

file = open(sys.argv[2], "w")
file.write(final_str)
