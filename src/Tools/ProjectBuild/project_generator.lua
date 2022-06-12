ToolProject("ProjectBuild", "ConsoleApp", true)
	debugdir "../../../workingdir/tools"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/fmt/include",
		"../../Dependencies/argparse"
	}

	local deps =
	{
		"Gaff",
		"EASTL"
	}

	dependson { "CodeGenerator", "Preprocessor" }
	dependson(deps)
	links(deps)

	defines { "FMT_HEADER_ONLY" }

	flags { "FatalWarnings" }

	SetupConfigMap()

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}

if not _OPTIONS["generate-preproc"] then
	project "RunProjectBuild"
		location(GetToolsLocation())

		kind "Makefile"

		dependson({ "ProjectBuild" })

		rebuildcommands
		{
		}

		buildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools",
			"ProjectBuild%{cfg.buildtarget.suffix} generate_headers",
			"ProjectBuild%{cfg.buildtarget.suffix} preprocessor",
			"ProjectBuild%{cfg.buildtarget.suffix} generate_project",
			"ProjectBuild%{cfg.buildtarget.suffix} update_modified_database"
		}

		cleancommands
		{
		}
end
