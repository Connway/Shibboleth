function SetIntermediateAndTargetDirs(configuration)
	if _ACTION then
		filter { "configurations:" .. configuration, "platforms:x86" }
			objdir("../.generated/build/" .. _ACTION .. "/intermediate")
			targetdir("../.generated/build/" .. _ACTION .. "/output/x86/" .. configuration)

		filter { "configurations:" .. configuration, "platforms:x64" }
			objdir("../.generated/build/" .. _ACTION .. "/intermediate")
			targetdir("../.generated/build/" .. _ACTION .. "/output/x64/" .. configuration)
	end
end

-- if os.get() == "windows" then
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

if _OPTIONS["gen-clang"] and _ACTION == "vs2017" then
	SetIntermediateAndTargetDirs("Debug_Clang")
	SetIntermediateAndTargetDirs("Release_Clang")
	SetIntermediateAndTargetDirs("Analyze_Clang")
	SetIntermediateAndTargetDirs("Profile_Clang")
	SetIntermediateAndTargetDirs("Optimized_Debug_Clang")
end

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
	defines { "NDEBUG" }
	optimize "Speed"
	runtime "Release"

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
	systemversion "10.0.17134.0"

filter { "system:windows", "platforms:x64" }
	defines { "WIN64" }

filter { "system:windows", "configurations:*Clang", "action:vs2017" }
	toolset "v141_clang_c2"

filter {}
