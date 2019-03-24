project "Resource"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("Resource")

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Frameworks/Gaff/include"
	}


project "ResourceModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ResourceModule.cpp" }

	ModuleCopy()

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

	filter {}

	ModuleIncludesAndLinks("Resource")
	NewDeleteLinkFix()
