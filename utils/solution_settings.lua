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

filter { "configurations:Debug*", "platforms:x86" }
	objdir "../build/intermediate"
	targetdir "../build/output/x86/Debug"

filter { "configurations:Debug*", "platforms:x64" }
	objdir "../build/intermediate"
	targetdir "../build/output/x64/Debug"

filter { "configurations:Release*", "platforms:x86" }
	objdir "../build/intermediate"
	targetdir "../build/output/x86/Release"

filter { "configurations:Release*", "platforms:x64" }
	objdir "../build/intermediate"
	targetdir "../build/output/x64/Release"

filter { "configurations:Analyze", "platforms:x86" }
	objdir "../build/intermediate"
	targetdir "../build/output/x86/Analyze"

filter { "configurations:Analyze", "platforms:x64" }
	objdir "../build/intermediate"
	targetdir "../build/output/x64/Analyze"

filter { "configurations:Profile", "platforms:x86" }
	objdir "../build/intermediate"
	targetdir "../build/output/x86/Profile"

filter { "configurations:Profile", "platforms:x64" }
	objdir "../build/intermediate"
	targetdir "../build/output/x64/Profile"

filter { "configurations:Debug*", "action:gmake", "options:not debug_optimization" }
	optimize "Off"

filter { "configurations:Debug*", "action:gmake", "options:debug_optimization" }
	optimize "Debug"

filter { "options:simd_set_aligned"}
	defines { "SIMD_SET_ALIGNED" }

filter { "action:vs*", "configurations:Analyze"}
	buildoptions { "/analyze" }

filter { "configurations:Profile" }
	defines { "SHIB_PROFILE" }

filter { "action:vs*" }
	buildoptions { "/sdl" }

filter { "system:windows" }
	defines { "WIN32", "_WINDOWS" }

filter { "system:windows", "configurations:*Clang" }
	toolset "msc-llvm-vs2014"

filter { "system:windows", "configurations:not *Clang" }
	toolset "v140"

filter {}

configuration "Debug*"
	defines { "_DEBUG", "DEBUG" }
	optimize "Off"

configuration "Release*"
	flags { "LinkTimeOptimization", "ReleaseRuntime" }
	defines { "NDEBUG" }
	optimize "Speed"

configuration {}
