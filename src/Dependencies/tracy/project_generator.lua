project "TracyClient"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	local build_files_in_dir = function(dir)
		files
		{
			dir .. "/**.h",
			dir .. "/**.cpp",
			dir .. "/**.hpp",
			dir .. "/**.c",
		}
	end

	kind "SharedLib"
	language "C++"
	warnings "Extra"

	files { "*.h", "*.hpp", "*.cpp", "*.c" }
	removefiles { "TracyClient.cpp" }

	build_files_in_dir("client")
	build_files_in_dir("common")

	defines
	{
		"TRACY_ENABLE",
		"TRACY_CALLSTACK",
		"TRACY_FIBERS",
		"TRACY_EXPORTS"
	}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	filter { "system:windows" }
		links { "ws2_32", "Dbghelp" }

	filter {}

	SetupConfigMap()

project "TracyProfiler"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	local build_files_in_dir = function(dir)
		files
		{
			dir .. "/**.h",
			dir .. "/**.cpp",
			dir .. "/**.hpp",
			dir .. "/**.c",
		}
	end

	kind "WindowedApp"
	language "C++"
	warnings "Extra"

	build_files_in_dir("common")
	build_files_in_dir("nfd")
	build_files_in_dir("imgui") -- tracy is using a beta branch of imgui.
	build_files_in_dir("server")
	build_files_in_dir("profiler")

	-- tracy is normally built with freetype used in imgui.
	-- I do not want to add another dependency, so I am using the default font renderer instead.
	removefiles { "imgui/misc/freetype/*.*" }

	includedirs
	{
		"profiler",
		"imgui",

		"../capstone/include/capstone",
		"../capstone/include",
		"../glfw/include",
		"."
	}

	local deps =
	{
		"capstone",
		"glfw",
		"zstd"
	}

	dependson(deps)
	links(deps)

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}


	filter { "system:not windows" }
		removefiles { "nfd/nfd_win.cpp" }

	filter { "system:windows" }
		removefiles { "nfd/nfd_gtk.c" }

		defines
		{
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX",
			"_CRT_SECURE_NO_WARNINGS",
			"_SCL_SECURE_NO_WARNINGS"
		}

		links { "ws2_32", "Dbghelp" }

	filter { "action:vs*" }
		buildoptions { "/bigobj" }

	filter {}

	SetupConfigMap()