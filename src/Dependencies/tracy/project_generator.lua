project "TracyClient"
	location(GetDependenciesLocation())

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
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	filter { "system:not windows" }
		build_files_in_dir("libbacktrace")

	filter { "system:not macosx"}
		removefiles { "libbacktrace/macho.cpp" }

	filter { "system:not linux" }
		removefiles { "libbacktrace/elf.cpp" }

	filter { "system:windows" }
		links { "ws2_32", "Dbghelp" }

	filter {}

	SetupConfigMap()

project "TracyProfiler"
	location(GetDependenciesLocation())

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
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}

	filter { "system:linux", "options:not wayland" }
		defines { "DISPLAY_SERVER_X11" }
		buildoptions { "`pkg-config --cflags gtk+-3.0`" }
		linkoptions { "`pkg-config --libs gtk+-3.0`" }

	filter { "system:linux", "options:not wayland", "architecture:x64" }
		includedirs { "/usr/lib/x86_64-linux-gnu/glib-2.0/include" }

	filter { "system:linux", "options:wayland" }
		defines { "DISPLAY_SERVER_WAYLAND" }

	filter { "system:not linux" }
		removefiles { "nfd/nfd_gtk.c" }

	filter { "system:not windows" }
		removefiles { "nfd/nfd_win.cpp" }

	filter { "system:macosx" }
		files { "nfd/nfd_cocoa.m" }

	filter { "system:windows" }

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
