project "LuaJIT"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	kind "Utility"
	language "C"
	warnings "Default"

	filter { "action:vs*", "configurations:Debug", "platforms:x86" }
		prebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86 debug ..\\..\\build\\output\\x86\\Debug"
		}

	filter { "action:vs*", "configurations:Release", "platforms:x86" }
		prebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86 release ..\\..\\build\\output\\x86\\Release"
		}

	filter { "action:vs*", "configurations:Debug", "platforms:x64" }
		prebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86_64 debug ..\\..\\build\\output\\x64\\Debug"
		}

	filter { "action:vs*", "configurations:Release", "platforms:x64" }
		prebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86_64 release ..\\..\\build\\output\\x64\\Release"
		}

	filter {}
