project "CodeGenerator"
	location(GetToolsLocation())

	kind "ConsoleApp"
	debugdir "../../../workingdir/tools"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
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
