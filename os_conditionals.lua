filter { "options:brofiler", "system:windows", "kind:StaticLib" }
	includedirs { "../../dependencies/brofiler/include" }

filter { "options:brofiler", "system:windows", "kind:SharedLib or ConsoleApp or WindowedApp" }
	includedirs { "../../dependencies/brofiler/include" }
	dependson { "brofiler" }
	links { "brofiler" }

filter {}
