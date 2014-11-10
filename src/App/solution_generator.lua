solution "App"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/app")
	end

	platforms { "x86", "x64" }
	configurations { "Debug", "Release" }
	warnings "Extra"
	flags { "Unicode", "NoRTTI" }

	nativewchar "Default"
	floatingpoint "Fast"
	startproject "App"
	debugdir "../../workingdir"

	filter { "platforms:x86" }
		architecture "x32"
		vectorextensions "SSE2"

	filter { "platforms:x64" }
		architecture "x64"

	filter { "configurations:Debug", "platforms:x86" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x86/Debug"

	filter { "configurations:Debug", "platforms:x64" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x64/Debug"

	filter { "configurations:Release", "platforms:x86" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x86/Release"

	filter { "configurations:Release", "platforms:x64" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x64/Release"

	filter { "language:C++", "action:gmake" }
		buildoptions { "-std=c++11", "-x c++" }

	filter { "configurations:Debug", "action:gmake", "options:not debug_optimization" }
		optimize "Off"

	filter { "configurations:Debug", "action:gmake", "options:debug_optimization" }
		optimize "Debug"

	filter {}

	configuration "vs*"
		buildoptions { "/sdl" }

	configuration "Debug"
		flags { "Symbols" }
		defines { "_DEBUG", "DEBUG" }

	configuration "Release"
		flags { "LinkTimeOptimization", "ReleaseRuntime" }
		optimize "Speed"

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")

	project "App"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/app")
		end

		if _OPTIONS["console_app"] then
			kind "ConsoleApp"
		else
			kind "WindowedApp"
		end

		language "C++"

		files { "**.h", "**.cpp", "**.inl" }

		includedirs
		{
			"../Shared/include",
			"../../dependencies/jansson",
			"../../dependencies/Gaff/include",
			"../../dependencies/utf8-cpp"
		}

		dependson
		{
			"Shared", "Gaff", "jansson", "Memory"
		}

		links
		{
			"Shared", "Gaff", "jansson", "Memory"
		}
