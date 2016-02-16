project "LuaJIT"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "Makefile"

	filter { "action:vs*", "configurations:Debug", "platforms:x86" }
		buildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86 debug ..\\..\\build\\output\\x86\\Debug"
		}

		cleancommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\debug\\x86\\lua51.*",
			"del ..\\..\\build\\output\\x86\\Debug\\lua51.*"
		}

		rebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\debug\\x86\\lua51.*",
			"del ..\\..\\build\\output\\x86\\Debug\\lua51.*",
			"build_luajit.bat x86 debug ..\\..\\build\\output\\x86\\Debug"
		}

	filter { "action:vs*", "configurations:Release", "platforms:x86" }
		buildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86 release ..\\..\\build\\output\\x86\\Release"
		}

		cleancommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\release\\x86\\lua51.*",
			"del ..\\..\\build\\output\\x86\\Release\\lua51.*"
		}

		rebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\release\\x86\\lua51.*",
			"del ..\\..\\build\\output\\x86\\Release\\lua51.*",
			"build_luajit.bat x86 release ..\\..\\build\\output\\x86\\Release"
		}

	filter { "action:vs*", "configurations:Debug", "platforms:x64" }
		buildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86_64 debug ..\\..\\build\\output\\x64\\Debug"
		}

		cleancommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\debug\\x86_64\\lua51.*",
			"del ..\\..\\build\\output\\x64\\Debug\\lua51.*"
		}

		rebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\debug\\x86_64\\lua51.*",
			"del ..\\..\\build\\output\\x64\\Debug\\lua51.*",
			"build_luajit.bat x86_64 debug ..\\..\\build\\output\\x64\\Debug"
		}

	filter { "action:vs*", "configurations:Release", "platforms:x64" }
		buildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"build_luajit.bat x86_64 release ..\\..\\build\\output\\x64\\Release"
		}

		cleancommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\release\\x86_64\\lua51.*",
			"del ..\\..\\build\\output\\x64\\Release\\lua51.*"
		}

		rebuildcommands
		{
			"cd ../../../dependencies/LuaJIT",
			"del lib\\release\\x86_64\\lua51.*",
			"del ..\\..\\build\\output\\x64\\Release\\lua51.*",
			"build_luajit.bat x86_64 release ..\\..\\build\\output\\x64\\Release"
		}

	filter {}
