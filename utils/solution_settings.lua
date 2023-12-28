startproject "Game_App"

exceptionhandling "SEH"
characterset "Unicode"
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
	"GLM_FORCE_INTRINSICS",
	"ZLIB_COMPAT",
	"LUAJIT_NUMMODE=2",
	"JPH_USE_LZCNT",
	"JPH_USE_TZCNT",
	"JPH_USE_FMADD"
}

filter { "system:macosx"}
	defaultplatform "arm64"
	platforms { "arm64" }

filter { "system:not macosx" }
	defaultplatform "x64"
	platforms { "x64" }

filter { "platforms:x64" }
	defines
	{
		--"GLM_FORCE_SSE42",
		--"JPH_USE_LZCNT",
		--"JPH_USE_TZCNT",
		--"JPH_USE_FMADD",
		"JPH_USE_SSE4_1",
		"JPH_USE_SSE4_2",
		--[["JPH_USE_AVX2",
		"JPH_USE_AVX",
		"JPH_USE_F16C"--]]
		"GLM_FORCE_SILENT_WARNINGS"
	}

	vectorextensions "SSE4.2"
	architecture "x64"

filter { "platforms:arm64" }
	vectorextensions "Default"
	architecture "ARM64"

filter {}

objdir(GetIntermediateLocation())
targetdir(GetOutputLocation())

dofile("module_suffix.lua")

filter { "configurations:*Debug*" }
	optimize "Debug"
	runtime "Debug"

	defines
	{
		"_DEBUG",
		"DEBUG",
		"SHIB_PROFILE",
		"JPH_PROFILE_ENABLED",
		"JPH_DEBUG_RENDERER",
		"JPH_FLOATING_POINT_EXCEPTIONS_ENABLED"
	}

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
	defines { "SHIB_PROFILE" }

filter { "configurations:*Release*" }
	--defines { "SHIB_NO_EDITOR" }

filter { "system:windows" }
	defines { "WIN32", "_WINDOWS" }

filter { "system:windows", "platforms:x64" }
	defines { "WIN64" }

filter { "system:linux" }

filter { "system:windows or linux"}
	systemversion "latest" -- this breaks on mac

filter { "system:macosx" }
	buildoptions { "-fchar8_t" }

filter { "platforms:x64" }

filter { "action:vs*" }
	buildoptions { "/Zc:__cplusplus", "/permissive-", "/Zc:static_assert-" }

filter {}
