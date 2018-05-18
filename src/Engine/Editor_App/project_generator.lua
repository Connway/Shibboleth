project "Editor_App"
	if _ACTION then
		location(GetEngineLocation())
	end

	debugdir "../../../workingdir"
	kind "WindowedApp"
	language "C++"
	targetname "Editor"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Editor/include",
		"../Shared/include",
		"../Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include"
	}

	local deps =
	{
		"Shared",
		"Memory",
		"Editor",
		"Gaff",
		"EASTL",
		"libjpeg",
		"libpng",
		"zlib-ng",
		"wxBase",
		"wxCore",
		"wxAUI",
		"wxADV"
	}

	IncludeWxWidgets()

	dependson(deps)
	links(deps)

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../Dependencies/dirent" }
		links
		{
			"comctl32",
			"Rpcrt4",
			"Dbghelp"
		}

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../workingdir"
	}
