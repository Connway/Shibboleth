project "JSONToMessagePack"
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
		"../../Dependencies/mpack",
		"../../Dependencies/EASTL/include"
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
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}
