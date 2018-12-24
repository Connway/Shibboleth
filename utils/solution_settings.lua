function SetIntermediateAndTargetDirs(configuration)
	if _ACTION then
		filter { "configurations:" .. configuration, "platforms:x86" }
			objdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/intermediate")
			targetdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x86/" .. configuration)

		filter { "configurations:" .. configuration, "platforms:x64" }
			objdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/intermediate")
			targetdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. configuration)
	end
end

-- if os.ishost("windows") then
-- 	platforms { "x86", "x64" }
-- else
	platforms { "x64" }
-- end

warnings "Extra"
characterset "Unicode"
exceptionhandling "SEH"
rtti "Off"

nativewchar "Default"
floatingpoint "Fast"
startproject "Game_App"
symbols "On"

filter { "action:not vs*" }
	cppdialect "C++17"

filter { "action:vs*" }
	buildoptions { "/std:c++17" }

filter {}

defines
{
	"UNICODE", "_UNICODE",
	"EA_COMPILER_NO_RTTI"
}

-- filter { "platforms:x86" }
-- 	architecture "x32"
-- 	vectorextensions "SSE2"

filter { "platforms:x64" }
	architecture "x64"

SetIntermediateAndTargetDirs("Debug")
SetIntermediateAndTargetDirs("Release")
SetIntermediateAndTargetDirs("Analyze")
SetIntermediateAndTargetDirs("Profile")
SetIntermediateAndTargetDirs("Optimized_Debug")

SetIntermediateAndTargetDirs("Debug_Clang")
SetIntermediateAndTargetDirs("Release_Clang")
SetIntermediateAndTargetDirs("Analyze_Clang")
SetIntermediateAndTargetDirs("Profile_Clang")
SetIntermediateAndTargetDirs("Optimized_Debug_Clang")

dofile("module_suffix.lua")

filter { "configurations:Debug*", "toolset:gcc", "options:not debug_optimization" }
	optimize "Off"

filter { "configurations:Debug*", "toolset:gcc", "options:debug_optimization" }
	optimize "Debug"

filter { "configurations:Debug*", "toolset:not gcc" }
	optimize "Debug"

filter { "configurations:Debug* or Optimized_Debug*" }
	defines { "_DEBUG", "DEBUG" }

filter { "configurations:Release* or Profile* or Analyze*" }
	flags { "LinkTimeOptimization" }
	optimize "Speed"
	runtime "Release"

	defines
	{
		"_HAS_ITERATOR_DEBUGGING=0",
		"_ITERATOR_DEBUG_LEVEL=0",
		"NDEBUG"
	}


filter { "configurations:Optimized_Debug*" }
	flags { "LinkTimeOptimization" }
	optimize "Speed"
	runtime "Release"

filter { "configurations:Profile" }
	defines { "SHIB_PROFILE" }

filter { "action:vs*", "configurations:Analyze*"}
	buildoptions { "/analyze" }

filter { "action:vs*" }
	buildoptions { "/sdl" }

filter { "system:windows" }
	defines { "WIN32", "_WINDOWS" }
	systemversion "latest"

filter { "system:windows", "platforms:x64" }
	defines { "WIN64" }

filter { "configurations:*Clang", "action:vs*" }
	toolset "msc-llvm-vs2014"

filter { "configurations:*Clang", "action:not vs*" }
	toolset "gcc"

filter { "configurations:*Clang", "action:not vs*" }
	toolset "clang"

filter {}
