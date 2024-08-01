if _OPTIONS["preproc-pipeline"] and not _OPTIONS["generate-preproc"] then
	Group "Tools/Unused"

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

	if _OPTIONS["preproc-pipeline"] then
		project "RunProjectBuild"
			location(GetToolsLocation())

			kind "Makefile"

			dependson({ "ProjectBuild", "BuildDependencies" })

			-- No rebuild commands for now.
			--[[
			rebuildcommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg} --clean",
				-- $TODO: Delete preproc folder
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg}"
			}
			--]]

			local gen_proj_options = ""

			buildcommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} generate_headers",
				"ProjectBuild%{cfg.buildtarget.suffix} preprocessor",
				"ProjectBuild%{cfg.buildtarget.suffix} generate_project" .. gen_proj_options,
				--"ProjectBuild%{cfg.buildtarget.suffix} update_modified_database",
				"ProjectBuild%{cfg.buildtarget.suffix} build --config %{cfg.buildcfg}"
			}

			-- Cleaning the ENTIRE project.
			cleancommands
			{
				"{RMDIR} ../../../../../.generated/build/" .. os.target() .. "/" .. _ACTION,
				"{RMDIR} ../../../../../.generated/preproc"
			}

		project "BuildDependencies"
			location(GetToolsLocation())

			kind "None"

			dependson(GetAllDependencies())
			dependson(GetAllFrameworks())
	end

	Group "Tools"
end
