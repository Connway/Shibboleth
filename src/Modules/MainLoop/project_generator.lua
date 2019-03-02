project "MainLoop"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_MainLoopModule.cpp" }

	flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../Dependencies/dirent" }

	filter {}

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

	includedirs
	{
		"include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/EASTL/include"
	}

	local deps = {
		"MainLoop",
		"Engine",
		"Gaff",
		"Memory",
		"EASTL",
		"mpack"
	}

	dependson(deps)
	links(deps)

	ModuleGen("MainLoop")
	ModuleCopy()
