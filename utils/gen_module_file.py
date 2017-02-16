import sys
import re
import os

gen_file = """// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

// Includes
{0}
#include <Gaff_ReflectionInterfaces.h>
#include <Gaff_EnumReflection.h>
#include <Gaff_Reflection.h>

namespace Gen
{{
	// Using namespaces.
{1}
	void InitReflection(void)
	{{
		// Initialize Enums.
{2}
		Gaff::IEnumReflection* enum_head = Gaff::GetEnumReflectionChainHead();

		while(enum_head) {{
			enum_head->init();
			enum_head = enum_head->next;
		}}

		// Initialize Attributes.
{3}
		Gaff::IReflection* head = Gaff::GetAttributeReflectionChainHead();

		while (head) {{
			head->init();
			head = head->attr_next;
		}}

{4}
		// Initialize any other reflection that we reference, but isn't owned by our module.
		head = Gaff::GetReflectionChainHead();

		while (head) {{
			head->init();
			head = head->next;
		}}
	}}
}}
"""

file_class_map = {}
file_enum_map = {}
namespaces = []

def ParseFile(root, file):
	f = open(os.path.join(root, file), "r")

	if not f:
		print("Failed to open file '" + file + "'!")
		return

	for line in f:
		namespace = ""

		m = re.search("NS_(.+)", line)

		if m and "END" not in m.group(1):
			namespace = m.group(1)
			namespace = namespace.lower()
			namespace = namespace[0:1].upper() + namespace[1:]

		if len(namespace):
			if namespace not in namespaces:
				namespaces.append(namespace)

		m = re.search("SHIB_REFLECTION_DECLARE\((.+)\)", line)

		if m:
			if file not in file_class_map:
				file_class_map[file] = []

			class_name = m.group(1)
			file_class_map[file].append(class_name)

		else:
			m = re.search("SHIB_ENUM_REFLECTION_DECLARE\((.+)\)", line)

			if m:
				if file not in file_enum_map:
					file_enum_map[file] = []

				enum_name = m.group(1)
				file_enum_map[file].append(enum_name)


module_dir = sys.argv[1]

if not os.path.exists(module_dir):
	print("Module directory '" + module_dir + "' not found!")
	exit()

if not os.path.isdir(module_dir):
	print("Module directory '" + module_dir + "' is not a directory!")
	exit()

for root, _, files in os.walk(module_dir):
	for f in files:
		if f.endswith(".h") and not "Gen_ReflectionInit.h" in f:
			ParseFile(root, f)

include_files = ""
using_namespaces = ""
init_attr_funcs = ""
init_enum_funcs = ""
init_funcs = ""

for file, classes in file_class_map.items():
	include_files = include_files + "#include <" + file + ">\n"

	for c in classes:
		if c.endswith("Attribute"):
			init_attr_funcs = init_attr_funcs + "\t\tReflection<" + c + ">::Init();\n"
		else:
			init_funcs = init_funcs + "\t\tReflection<" + c + ">::Init();\n"

for file, enums in file_enum_map.items():
	if file not in file_class_map:
		include_files = include_files + "#include <" + file + ">\n"

	for e in enums:
		init_enum_funcs = init_enum_funcs + "\t\tEnumReflection<" + e + ">::Init();\n"

for n in namespaces:
	using_namespaces = using_namespaces + "\tusing namespace " + n + ";\n"

output = gen_file.format(include_files, using_namespaces, init_enum_funcs, init_attr_funcs, init_funcs)
output_file = os.path.join(module_dir, "include", "Gen_ReflectionInit.h")

f = None

if os.path.isfile(output_file):
	f = open(output_file, "r")

if not f or not output in f:
	f = open(output_file, "w")
	f.write(output)
