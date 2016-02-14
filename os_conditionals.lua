includedirs { "dependencies/brofiler/include" }

filter { "options:brofiler", "system:windows", "kind:StaticLib" }
	defines { "USE_PROFILER" }

filter { "options:brofiler", "system:windows", "kind:not StaticLib" }
	defines { "USE_PROFILER" }
	dependson { "brofiler" }
	links { "brofiler" }

filter {}
