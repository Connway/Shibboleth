local gen_file = [[
// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

// Includes
%s
#include <Gaff_ReflectionInterfaces.h>
#include <Gaff_EnumReflection.h>
#include <Gaff_Reflection.h>

namespace Gen
{
	// Using namespaces.
%s
	void InitReflection(void)
	{
		// Initialize Enums.
%s
		Gaff::IEnumReflection* enum_head = Gaff::GetEnumReflectionChainHead();

		while(enum_head) {
			enum_head->init();
			enum_head = enum_head->next;
		}

		// Initialize Attributes.
%s
		Gaff::IReflection* head = Gaff::GetAttributeReflectionChainHead();

		while (head) {
			head->init();
			head = head->attr_next;
		}

%s
		// Initialize any other reflection that we reference, but isn't owned by our module.
		head = Gaff::GetReflectionChainHead();

		while (head) {
			head->init();
			head = head->next;
		}

		// Register our module as the owners of the reflection.
%s	}
}
]]

newoption
{
	trigger = "module",
	value = "NAME",
	description = "The name of the module to generate the 'Gen_ReflectionInit.h' file for. ('gen_module_file' only)"
}

newaction
{
	trigger = "gen_module_file",
	description = "Generate the 'Gen_ReflectionInit.h' file for a module.",
	execute = function()
		local source_folder = "../src/Modules/" .. _OPTIONS["module"]
		local include_folder = source_folder .. "/include"

		local file_class_map = {}
		local file_enum_map = {}
		local namespaces = {}

		function ParseFile(file)
			local stripped_file = file:sub(include_folder:len() + 2)

			if file == "Gen_ReflectionInit.h" then
				return
			end

			local lines = io.readfile(file):explode("\n")

			for _, line in next, lines do
				local match = line:match("NS_(.+)")

				if match then
					if match:find("END") == nil then
						local namespace = match:sub(1, 1) .. match:sub(2, -2):lower()

						for _,v in pairs(namespaces) do
							if v:match(namespace) then
								goto continue
							end
						end

						table.insert(namespaces, namespace)
					end

					goto continue
				end

				-- Detect classes
				match = line:match("SHIB_REFLECTION_DECLARE%((.+)%)")

				if match then
					if not file_class_map[stripped_file] then
						file_class_map[stripped_file] = {}
					end

					table.insert(file_class_map[stripped_file], match)
					goto continue
				end

				-- Detect enums
				match = line:match("SHIB_ENUM_REFLECTION_DECLARE%((.+)%)")

				if match then
					if not file_enum_map[stripped_file] then
						file_enum_map[stripped_file] = {}
					end

					table.insert(file_enum_map[stripped_file], match)
					goto continue
				end

				::continue::
			end
		end


		if os.isfile(source_folder) then
			print("Module directory is actually a file!")
			return
		end

		if os.isfile(include_folder) then
			print("Module include directory is actually a file!")
			return
		end

		if not os.isdir(source_folder) then
			print("Module directory does not exist!")
			return
		end

		if not os.isdir(include_folder) then
			print("Module include directory does not exist!")
			return
		end

		local headers = os.matchfiles(include_folder .. "/**.h")
		table.foreachi(headers, ParseFile)

		local include_files = ""
		local using_namespaces = ""
		local init_attr_funcs = ""
		local init_enum_funcs = ""
		local init_funcs = ""
		local module_registers = ""

		for _,n in pairs(namespaces) do
			using_namespaces = using_namespaces .. "\tusing namespace " .. n .. ";\n"
		end

		for k,v in pairs(file_class_map) do
			include_files = include_files .. "#include <" .. k .. ">\n"

			for _,c in pairs(v) do
				if c:endswith("Attribute") then
					init_attr_funcs = init_attr_funcs .. "\t\tReflection<" .. c .. ">::Init();\n"
				else
					init_funcs = init_funcs .. "\t\tReflection<" .. c .. ">::Init();\n"
				end

				if module_registers == "" then
					module_registers = "\t\tReflectionManager& refl_mgr = GetApp().getReflectionManager();\n\n"
				end

				module_registers = module_registers .. "\t\trefl_mgr.registerOwningModule(Reflection<" .. c .. ">::GetHash(), \"" .. _OPTIONS["module"] .. "\");\n"
			end
		end

		for k,v in pairs(file_enum_map) do
			if file_class_map[k] == nil then
				include_files = include_files .. "#include <" .. k .. ">\n"
			end

			for _,e in pairs(enums) do
				init_enum_funcs = init_enum_funcs .. "\t\tEnumReflection<" .. e .. ">::Init();\n"
			end

			if module_registers == "" then
				module_registers = "\t\tReflectionManager& refl_mgr = GetApp().getReflectionManager();\n\n"
			end

			module_registers = module_registers .. "\t\trefl_mgr.registerEnumOwningModule(EnumReflection<" .. e .. ">::GetHash(), \"" .. _OPTIONS["module"] .. "\");\n"
		end

		local file_path = include_folder .. "/Gen_ReflectionInit.h"
		io.writefile(file_path, gen_file:format(include_files, using_namespaces, init_enum_funcs, init_attr_funcs, init_funcs, module_registers))
	end
}
