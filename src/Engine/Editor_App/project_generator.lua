project "Editor_App"
	if _ACTION then
		location(GetEngineLocation())
	end

	debugdir "../../../workingdir"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

	includedirs
	{
		"include",
		"../Editor/include",
		"../Engine/include",
		"../Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include"
	}

	local deps =
	{
		"Engine",
		"Memory",
		"Editor",
		"Gaff",
		"EASTL",
		"libjpeg",
		"libpng",
		"zlib-ng",
		"mpack",
		"wxBase",
		"wxCore",
		"wxAUI"
	}

	IncludeWxWidgets(true)

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links
		{
			"comctl32",
			"Rpcrt4",
			"Dbghelp"
		}

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter { "configurations:Static_*" }
		defines { "SHIB_STATIC", "SHIB_EDITOR" }
		StaticLinks()

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir"
	}
