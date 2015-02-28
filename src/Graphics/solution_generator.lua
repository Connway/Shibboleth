solution "Graphics"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/graphics")
	end

	if os.get() == "windows" then
		configurations { "Debug_OpenGL", "Release_OpenGL", "Debug_Direct3D", "Release_Direct3D" }
	else
		configurations { "Debug_OpenGL", "Release_OpenGL" }
	end

	platforms { "x86", "x64" }
	warnings "Extra"
	flags { "Unicode", "NoRTTI", "SEH" } -- unfortunately need exceptions for crash handlers :(

	nativewchar "Default"
	floatingpoint "Fast"
	debugdir "../../workingdir"

	filter { "options:symbols" }
		flags { "Symbols" }
		-- defines { "SYMBOL_BUILD" }

	-- filter { "system:windows", "options:symbols" }
	-- 	links { "Dbghelp" }

	filter { "options:ltcg" }
		flags { "LinkTimeOptimization", "NoIncrementalLink" }

	filter { "platforms:x86" }
		architecture "x32"
		vectorextensions "SSE2"

	filter { "platforms:x64" }
		architecture "x64"

	filter { "configurations:Debug*", "platforms:x86" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x86/Debug"

	filter { "configurations:Debug*", "platforms:x64" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x64/Debug"

	filter { "configurations:Release*", "platforms:x86" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x86/Release"

	filter { "configurations:Release*", "platforms:x64" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x64/Release"

	filter { "language:C++", "action:gmake" }
		buildoptions { "-std=c++11", "-x c++" }

	filter { "configurations:Debug*", "action:gmake", "options:not debug_optimization" }
		optimize "Off"

	filter { "configurations:Debug*", "action:gmake", "options:debug_optimization" }
		optimize "Debug"

	filter { "configurations:*Direct3D"}
		defines { "USE_DX" }

	filter { "configurations:*OpenGL"}
		defines { "GLEW_STATIC" }

	filter { "configurations:Debug_Direct3D", "platforms:x86" }
		targetsuffix "_Direct3D32d"

	filter { "configurations:Release_Direct3D", "platforms:x86" }
		targetsuffix "_Direct3D32"

	filter { "configurations:Debug_OpenGL", "platforms:x86" }
		targetsuffix "_OpenGL32d"

	filter { "configurations:Release_OpenGL", "platforms:x86" }
		targetsuffix "_OpenGL32"

	filter { "configurations:Debug_Direct3D", "platforms:x64" }
		targetsuffix "_Direct3D64d"

	filter { "configurations:Release_Direct3D", "platforms:x64" }
		targetsuffix "_Direct3D64"

	filter { "configurations:Debug_OpenGL", "platforms:x64" }
		targetsuffix "_OpenGL64d"

	filter { "configurations:Release_OpenGL", "platforms:x64" }
		targetsuffix "_OpenGL64"

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	configuration "vs*"
		buildoptions { "/sdl" }

	configuration "Debug*"
		flags { "Symbols" }
		defines { "_DEBUG", "DEBUG" }

	configuration "Release*"
		flags { "LinkTimeOptimization", "ReleaseRuntime" }
		optimize "Speed"

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gleam/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")
	dofile("../../dependencies/glew/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")
	dofile("../Memory/include_external.lua")

	project "Graphics"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/graphics")
		end

		kind "SharedLib"
		language "C++"

		files { "**.h", "**.cpp", "**.inl" }

		includedirs
		{
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/utf8-cpp"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"Gleam", "glew", "Memory"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"Gleam", "glew", "Memory"
		}

		filter { "configurations:*OpenGL", "system:windows" }
			links { "OpenGL32" }

		filter { "configurations:*OpenGL", "system:not windows" }
			links { "gl" }

		filter { "configurations:*Direct3D", "system:windows" }
			links { "d3d11", "D3dcompiler", "dxgi" }

		filter {}