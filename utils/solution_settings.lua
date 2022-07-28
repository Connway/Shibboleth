function SetIntermediateAndTargetDirs(configuration)
	objdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/intermediate")

	filter { "configurations:" .. configuration, "platforms:x86" }
		targetdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x86/" .. configuration)

	filter { "configurations:" .. configuration, "platforms:x64" }
		targetdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. configuration)
end

startproject "Game_App"
defaultplatform "x64"

vectorextensions "SSE4.2"
exceptionhandling "SEH"
characterset "Unicode"
systemversion "latest"
nativewchar "Default"
floatingpoint "Fast"
cppdialect "C++20"
cdialect "C11"
warnings "Extra"
symbols "On"
rtti "Off"

defines
{
	"UNICODE", "_UNICODE",
	"EA_COMPILER_NO_RTTI",
	--"GLM_FORCE_SSE42",
	"GLM_FORCE_INTRINSICS",
	"ZLIB_COMPAT"
}

platforms { "x64" }

filter { "platforms:x64" }
	architecture "x64"

filter {}

for _,v in ipairs(configs) do
	SetIntermediateAndTargetDirs(v)
end

dofile("module_suffix.lua")

filter { "configurations:*Debug*" }
	optimize "Debug"
	runtime "Debug"

	defines { "_DEBUG", "DEBUG", "PROFILE" }

filter { "configurations:*Optimized_Debug* or *Release* or *Profile*" }
	flags { "LinkTimeOptimization" }
	optimize "Speed"
	runtime "Release"

filter { "configurations:*Release* or *Profile*" }
	defines
	{
		"_HAS_ITERATOR_DEBUGGING=0",
		"_ITERATOR_DEBUG_LEVEL=0",
		"NDEBUG"
	}

filter { "configurations:*Profile*" }
	defines { "PROFILE" }

filter { "system:windows" }
	defines { "WIN32", "_WINDOWS" }

filter { "system:windows", "platforms:x64" }
	defines { "WIN64" }

filter { "action:vs*" }
	buildoptions { "/Zc:__cplusplus", "/permissive-", "/Zc:static_assert-" }

filter {}
