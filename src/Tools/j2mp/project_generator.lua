project "j2mp"
	if _ACTION then
		location(GetToolsLocation())
	end

	kind "ConsoleApp"
	debugdir "../../../workingdir"
	language "C++"

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

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
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}
