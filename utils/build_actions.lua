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
	description = "The project to build. (Defaults to VS2017) ('build' only)",
	allowed =
	{
		{ "vs2017", "Visual Studio 2017" }
	}
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
		function VS2017(config)
			local cwd = os.getcwd()

			os.chdir("C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer")

			local path, _ = os.outputof("vswhere.exe -latest -property installationPath")
			os.chdir(path .. "\\VC\\Auxiliary\\Build")

			local solution = cwd .. "/../project/" .. project .. "/Shibboleth.sln"

			local msbuild = "msbuild \"" .. solution .. "\" /p:Configuration=" .. config
			local vcvars = "vcvarsall.bat amd64"

			if _OPTIONS["rebuild"] then
				msbuild = msbuild .. " /t:rebuild"
			elseif _OPTIONS["clean"] then
				msbuild = msbuild .. " /t:clean"
			end

			-- Have to execute all in one long command, since running os.execute("vcvarsall.bat amd64")
			-- does not carry over between os.execute() calls.
			os.execute(vcvars .. " && " .. msbuild)
		end

		function WindowsClang(config)
		end


		-- Start of action here.
		local project = _OPTIONS["project"]
		local config = _OPTIONS["config"]
		local cwd = os.getcwd()

		if not project then
			print("You must specify a project to build with '--project=[PROJ]'")
			return
		end

		if os.ishost("windows") then
			if project == "vs2017" then
				VS2017(config)
			elseif project == "clang" then
				WindowsClang(config)
			else
				print("Building project '" .. project .. "' not supported on Windows!")
			end

		else
			print "'build' action not supported on this OS!"
			return
		end

		-- Change back to the original working directory.
		os.chdir(cwd)
	end
}
