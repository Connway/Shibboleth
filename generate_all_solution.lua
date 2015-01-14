solution "All"
	if _ACTION then
		location ("project/" .. _ACTION)
	end

	platforms { "x86", "x64" }

	-- configurations { "Debug", "Release" }

	if os.get() == "windows" then
		configurations { "Debug_OpenGL", "Release_OpenGL", "Debug_Direct3D", "Release_Direct3D" }
	else
		configurations { "Debug_OpenGL", "Release_OpenGL" }
	end

	warnings "Extra"
	flags { "Unicode", "NoRTTI" }

	nativewchar "Default"
	floatingpoint "Fast"
	startproject "App" -- Not working for some reason. Might not work with externals?

	filter { "platforms:x86" }
		architecture "x32"
		vectorextensions "SSE2"

	filter { "platforms:x64" }
		architecture "x64"

	filter { "configurations:Debug*", "platforms:x86" }
		objdir "build/intermediate"
		targetdir "build/output/x86/Debug"

	filter { "configurations:Debug*", "platforms:x64" }
		objdir "build/intermediate"
		targetdir "build/output/x64/Debug"

	filter { "configurations:Release*", "platforms:x86" }
		objdir "build/intermediate"
		targetdir "build/output/x86/Release"

	filter { "configurations:Release*", "platforms:x64" }
		objdir "build/intermediate"
		targetdir "build/output/x64/Release"

	filter { "language:C++", "action:gmake" }
		buildoptions { "-std=c++11", "-x c++" }

	filter { "configurations:Debug*", "action:gmake", "options:not debug_optimization" }
		optimize "Off"

	filter { "configurations:Debug*", "action:gmake", "options:debug_optimization" }
		optimize "Debug"

	filter {}

	configuration "vs*"
		buildoptions { "/sdl" }

	configuration "Debug*"
		flags { "Symbols" }
		defines { "_DEBUG", "DEBUG" }

	configuration "Release*"
		flags { "LinkTimeOptimization", "ReleaseRuntime" }
		optimize "Speed"

	configuration {}

	local all_includes = os.matchfiles("src/**/all_solution_include.lua")

	for i = 1, table.getn(all_includes) do
		dofile(all_includes[i])
	end
