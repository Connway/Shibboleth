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

startproject "Game_App"
defaultplatform "x64"

exceptionhandling "SEH"
characterset "Unicode"
nativewchar "Default"
floatingpoint "Fast"
cppdialect "C++17"
warnings "Extra"
symbols "On"
rtti "Off"

defines
{
	"UNICODE", "_UNICODE",
	"EA_COMPILER_NO_RTTI",
	"GLM_FORCE_SSE42",
	"ZLIB_COMPAT"
}

-- vectorextensions "SSE4.2"

platforms { "x64" }

filter { "platforms:x64" }
	architecture "x64"

filter {}

SetIntermediateAndTargetDirs("Debug")
SetIntermediateAndTargetDirs("Release")
SetIntermediateAndTargetDirs("Profile")
SetIntermediateAndTargetDirs("Optimized_Debug")
SetIntermediateAndTargetDirs("Static_Debug_D3D11")
SetIntermediateAndTargetDirs("Static_Release_D3D11")
-- SetIntermediateAndTargetDirs("Static_Debug_Vulkan")
-- SetIntermediateAndTargetDirs("Static_Release_Vulkan")

SetIntermediateAndTargetDirs("Debug_Clang")
SetIntermediateAndTargetDirs("Release_Clang")
SetIntermediateAndTargetDirs("Profile_Clang")
SetIntermediateAndTargetDirs("Optimized_Debug_Clang")

dofile("module_suffix.lua")

filter { "configurations:*Debug*", "toolset:gcc", "options:not debug_optimization" }
	optimize "Off"

filter { "configurations:*Debug*", "toolset:gcc", "options:debug_optimization" }
	optimize "Debug"

filter { "configurations:*Debug*", "toolset:not gcc" }
	optimize "Debug"

filter { "toolset:clang"--[[, "rtti:off"--]] }
	defines { "_HAS_STATIC_RTTI=0" }

filter { "configurations:*Debug* or Optimized_Debug*" }
	defines { "_DEBUG", "DEBUG" }

filter { "configurations:Release* or Profile*" }
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

filter { "configurations:Profile*" }
	defines { "SHIB_PROFILE" }

filter { "action:vs*" }
	buildoptions { "/sdl" }

filter { "system:windows" }
	defines { "WIN32", "_WINDOWS" }
	systemversion "latest"
	disablewarnings "4307" -- For hash functions.

filter { "system:windows", "platforms:x64" }
	defines { "WIN64" }

filter { "configurations:*Clang", "action:vs*" }
	toolset "msc-llvm-vs2014"

filter { "configurations:not *Clang", "action:not vs*" }
	toolset "gcc"

filter { "configurations:*Clang", "action:not vs*" }
	toolset "clang"

filter {}
