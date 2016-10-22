function SetIntermediateAndTargetDirs(configuration)
	filter { "configurations:" .. configuration, "platforms:x86" }
		objdir "../build/intermediate"
		targetdir("../build/output/x86/" .. configuration)

	filter { "configurations:" .. configuration, "platforms:x64" }
		objdir "../build/intermediate"
		targetdir("../build/output/x64/" .. configuration)
end

-- if os.get() == "windows" then
-- 	platforms { "x86", "x64" }
-- else
	platforms { "x64" }
-- end

warnings "Extra"
characterset "Unicode"
exceptionhandling "SEH" -- unfortunately need exceptions for crash handlers :(
rtti "Off"

nativewchar "Default"
floatingpoint "Fast"
startproject "App"
symbols "On"

flags { "C++14" }

filter { "options:physx" }
	defines { "USE_PHYSX" }

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

if _OPTIONS["gen-clang"] and _ACTION == "vs2015" then
	SetIntermediateAndTargetDirs("Debug_Clang")
	SetIntermediateAndTargetDirs("Release_Clang")
	SetIntermediateAndTargetDirs("Analyze_Clang")
	SetIntermediateAndTargetDirs("Profile_Clang")
	SetIntermediateAndTargetDirs("Optimized_Debug_Clang")
end

filter { "configurations:Debug*", "toolset:gcc", "options:not debug_optimization" }
	optimize "Off"

filter { "configurations:Debug*", "toolset:gcc", "options:debug_optimization" }
	optimize "Debug"

filter { "configurations:Debug*", "toolset:not gcc" }
	optimize "Debug"

filter { "configurations:Debug* or Optimized_Debug*" }
	defines { "_DEBUG", "DEBUG" }

filter { "configurations:Release* or Profile* or Analyze*" }
	flags { "LinkTimeOptimization", "ReleaseRuntime" }
	defines { "NDEBUG" }
	optimize "Speed"

filter { "configurations:Optimized_Debug*" }
	flags { "LinkTimeOptimization", "ReleaseRuntime" }
	optimize "Speed"

filter { "configurations:Profile" }
	defines { "SHIB_PROFILE" }

filter { "options:simd_set_aligned"}
	defines { "SIMD_SET_ALIGNED" }

filter { "action:vs*", "configurations:Analyze"}
	buildoptions { "/analyze" }

filter { "action:vs*" }
	buildoptions { "/sdl" }

filter { "system:windows" }
	defines { "WIN32", "_WINDOWS" }

filter { "system:windows", "configurations:*Clang" }
	toolset "msc-llvm-vs2014"

filter { "system:windows", "configurations:not *Clang" }
	toolset "v140"

filter {}
