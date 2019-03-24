project "MainLoop"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("MainLoop")

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/EASTL/include"
	}

project "MainLoopModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_MainLoopModule.cpp" }

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../Dependencies/dirent" }

	filter {}

	ModuleIncludesAndLinks("MainLoop")
	ModuleCopy()
