project "CodeGenerator"
	if _ACTION then
		location(GetToolsLocation())
	end

	kind "ConsoleApp"
	debugdir "../../../workingdir/tools"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/argparse"
	}

	local deps =
	{
		"Gaff",
		"EASTL"
	}

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	SetupConfigMap()

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}
