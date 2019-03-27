project "easy_profiler"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"
	warnings "Default"

	defines
	{
		"EASY_CHRONO_HIGHRES_CLOCK",
		-- Currently causing compilation errors.
		-- "EASY_OPTION_MEASURE_STORAGE_EXPAND",
		"EASY_OPTION_STORAGE_EXPAND_BLOCKS_ON",
		"EASY_OPTION_IMPLICIT_THREAD_REGISTRATION",
		"EASY_OPTION_EVENT_TRACING_ENABLED",
		"EASY_OPTION_BUILTIN_COLORS",
		"EASY_PROFILER_STATIC",
		"EASY_DEFAULT_PORT=28077",
		"EASY_PROFILER_VERSION_MAJOR=1",
		"EASY_PROFILER_VERSION_MINOR=3",
		"EASY_PROFILER_VERSION_PATCH=0"
	}

	files { "easy_profiler_core/**.h", "easy_profiler_core/**.c", "easy_profiler_core/**.cpp" }
	includedirs { "easy_profiler_core/include" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
		characterset "MBCS"

	filter {}

	SetupConfigMap()
