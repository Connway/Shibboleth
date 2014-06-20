project "LuaJIT"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "SharedLib"
	language "C"
	warnings "Default"

	filter { "action:vs*", "configurations:Debug", "platforms:x86" }
		prebuildcommands
		{
			"cd ../../dependencies/LuaJIT-2.0.3",
			"build_luajit.bat x86 debug ..\\..\\build\\output\\x86\\Debug"
		}

	filter { "action:vs*", "configurations:Release", "platforms:x86" }
		prebuildcommands
		{
			"cd ../../dependencies/LuaJIT-2.0.3",
			"build_luajit.bat x86 release ..\\..\\build\\output\\x86\\Release"
		}

	filter { "action:vs*", "configurations:Debug", "platforms:x86_64" }
		prebuildcommands
		{
			"cd ../../dependencies/LuaJIT-2.0.3",
			"build_luajit.bat x86_64 debug ..\\..\\build\\output\\x86\\Debug"
		}

	filter { "action:vs*", "configurations:Release", "platforms:x86_64" }
		prebuildcommands
		{
			"cd ../../dependencies/LuaJIT-2.0.3",
			"build_luajit.bat x86 release ..\\..\\build\\output\\x86\\Release"
		}

	filter {}
