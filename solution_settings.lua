if os.get() == "windows" then
	platforms { "x86", "x64" }
else
	platforms { "x64" }
end

warnings "Extra"
flags { "Unicode" }
exceptionhandling "SEH" -- unfortunately need exceptions for crash handlers :(
rtti "Off"

nativewchar "Default"
floatingpoint "Fast"
startproject "App"
debugdir "workingdir/App"

filter { "options:symbols" }
	flags { "Symbols" }
	defines { "SYMBOL_BUILD" }

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

filter { "options:simd_set_aligned"}
	defines { "SIMD_SET_ALIGNED" }

filter {}

configuration "vs*"
	buildoptions { "/sdl" }

configuration "Debug*"
	flags { "Symbols" }
	defines { "_DEBUG", "DEBUG", "SYMBOL_BUILD" }

configuration "Release*"
	flags { "LinkTimeOptimization", "ReleaseRuntime" }
	optimize "Speed"

configuration {}
