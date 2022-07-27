ToolProject "JSONToMessagePack"
	debugdir "../../../workingdir/tools"
	language "C++"

	local source_dir = GetToolsSourceDirectory("JSONToMessagePack")
	local base_dir = GetToolsDirectory("JSONToMessagePack")

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

	IncludeDirs
	{
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Dependencies/rapidjson",
		base_dir .. "../../Dependencies/mpack",
		base_dir .. "../../Dependencies/EASTL/include"
	}

	local deps =
	{
		"Gaff",
		"EASTL",
		"mpack"
	}

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	postbuildcommands
	{
		"{MKDIR} ../../../../../../workingdir/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/tools"
	}
