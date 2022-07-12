if not _OPTIONS["generate-preproc"] then
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

		project "RunProjectBuild"
			location(GetToolsLocation())

			kind "Makefile"

			dependson({ "ProjectBuild" })

			rebuildcommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg} --clean",
				-- $TODO: Delete preproc folder
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg}"
			}

			buildcommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} generate_headers",
				"ProjectBuild%{cfg.buildtarget.suffix} preprocessor",
				"ProjectBuild%{cfg.buildtarget.suffix} generate_project",
				--"ProjectBuild%{cfg.buildtarget.suffix} update_modified_database",
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg}"
			}

			cleancommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg} --clean"
			}
end
