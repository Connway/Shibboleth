newoption
{
	trigger = "config",
	value = "CONFIG",
	description = "Configuration to build. ('build' only)",
	allowed = config_options,
	default = "Debug"
}

newoption
{
	trigger = "project",
	description = "The project to build. (Defaults to VS2022/gmake depending on platform) ('build' only)",
	allowed =
	{
		{ "vs2022", "Visual Studio 2022" },
		{ "gmake2", "GNU Make" }
	}
}

newoption
{
	trigger = "target",
	description = "The target to build. ('build' only)"
}

newoption
{
	trigger = "clean",
	description = "Clean the build outputs. ('build' only)"
}

newoption
{
	trigger = "rebuild",
	description = "Cleans the build outputs and builds the project. ('build' only)"
}

newaction
{
	trigger = "build",
	description = "Builds everything. Defaults to Debug builds with the platform default compiler.",
	execute = function()
		function VS2022(config, target)
			local solution = os.getcwd() .. "/../.generated/project/" .. os.target() .. "/vs2022/Shibboleth.sln"
			local msbuild = "msbuild \"" .. solution .. "\" /p:Configuration=" .. config
			local vcvars = "vcvarsall.bat amd64"

			os.chdir("C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer")

			local path, _ = os.outputof("vswhere.exe -latest -property installationPath")
			os.chdir(path .. "\\VC\\Auxiliary\\Build")

			-- Have to execute all in one long command, since running os.execute("vcvarsall.bat amd64")
			-- does not carry over between os.execute() calls.
			if _OPTIONS["rebuild"] then
				if target then
					msbuild = msbuild .. " /t:" .. target .. ":Rebuild"
				else
					msbuild = msbuild .. " /t:rebuild"
				end
			elseif _OPTIONS["clean"] then
				if target then
					msbuild = msbuild .. " /t:" .. target .. ":Clean"
				else
					msbuild = msbuild .. " /t:clean"
				end
			else
				if target then
					msbuild = msbuild .. " /t:" .. target
				end
			end

			os.execute(vcvars .. " && " .. msbuild)
		end

		function Make(config, target)
			local make_dir = os.getcwd() .. "/../.generated/project/" .. os.target() .. "/gmake2"
			local make_cmd = "gmake "

			if not target then
				target = "all"
			end

			make_cmd = make_cmd .. "config=" .. string.lower(config) .. "_x64"
			os.chdir(make_dir)

			if _OPTIONS["rebuild"] then
				os.execute(make_cmd .. " clean")
				os.execute(make_cmd .. " " .. target)
			elseif _OPTIONS["clean"] then
				os.execute(make_cmd .. " clean")
			else
				os.execute(make_cmd .. " " .. target)
			end
		end


		-- Start of action here.
		local project = _OPTIONS["project"]
		local config = _OPTIONS["config"]
		local target = _OPTIONS["target"]
		local cwd = os.getcwd()

		if not project then
			if os.ishost("windows") then
				project = "vs2022"
			elseif os.ishost("linux") then
				project = "gmake"
			else
				print("You must specify a project to build with '--project=[PROJ]'")
				return
			end
		end

		if not config then
			print("You must specify a configuration to use with '--config=[CONFIG]'")
		end

		local cwd = os.getcwd()

		if os.ishost("windows") then
			if project == "vs2022" then
				VS2022(config, target)
			else
				print("Building project '" .. project .. "' not supported on Windows!")
			end

		elseif os.ishost("linux") then
			if project == "gmake2" then
				Make(config, target)
			else
				print("Building project '" .. project .. "' not supported on Linux!")
			end

		else
			print("'build' action not supported on '" .. os.target() .. "'!")
		end

		-- Change back to the original working directory.
		os.chdir(cwd)
	end
}
