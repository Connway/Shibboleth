newoption
{
	trigger = "pull_vs_proj",
	description = "Cleans the build outputs and builds the project. ('build' only)"
}

newoption
{
	trigger = "pull_out_file",
	description = "The file to write the results to."
}

newoption
{
	trigger = "pull_root_dir",
	description = "The root directory of the project file so we can trim the file path."
}

newaction
{
	trigger = "pull_files_from_vs",
	description = "Generates a *_files.lua file from a Visual Studio project file.",
	execute = function()
		local proj_file = _OPTIONS["pull_vs_proj"]
		local out_file = _OPTIONS["pull_out_file"]
		local root_dir = _OPTIONS["pull_root_dir"]

		if _OPTIONS["pull_vs_proj"] == nil then
			print("No VS project file specified!")
			return
		end

		if _OPTIONS["pull_out_file"] == nil then
			print("No output file specified!")
			return
		end

		if _OPTIONS["pull_root_dir"] == nil then
			print("No project root directory specified!")
			return
		end

		proj_file = proj_file:gsub("\\", "/")
		out_file = out_file:gsub("\\", "/")
		root_dir = root_dir:gsub("\\", "/")

		if root_dir:sub(-1) ~= "/" then
			root_dir = root_dir .. "/"
		end

		local start,_ = _OPTIONS["pull_vs_proj"]:find(_OPTIONS["pull_root_dir"])

		if start ~= 1 then
			print("The project file does not live in the root directory!")
			return
		end

		if not os.isfile(_OPTIONS["pull_vs_proj"]) then
			print("VS project is not a file!")
			return
		end

		local lines = io.readfile(_OPTIONS["pull_vs_proj"]):explode("\n")

		local files = ""

		for _,line in next, lines do
			local file = line:match("ClInclude Include=\"(.+)\"")

			if file ~= nil then
				files = files .. "\t\"" .. file:gsub("\\", "/"):gsub(root_dir, "") .. "\",\n"
			end

			file = line:match("ClCompile Include=\"(.+)\"")

			if file ~= nil then
				files = files .. "\t\"" .. file:gsub("\\", "/"):gsub(root_dir, "") .. "\",\n"
			end
		end

		files = "files\n{\n" .. files:sub(1, -3) .. "\n}\n"
		io.writefile(out_file, files)
	end
}
