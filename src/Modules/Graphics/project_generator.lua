group "Modules/Graphics"

project "Graphics"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/graphics")
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_GraphicsModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		-- "../../../dependencies/rapidjson",
		-- "../../../dependencies/glm",
		-- "../../../dependencies/mpack",
		"../../../frameworks/Gaff/include",
		"../../../frameworks/Gleam/include"
	}


project "GraphicsModule"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/graphics")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_GraphicsModule.cpp" }

	ModuleGen("Graphics")
	ModuleCopy()

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

	filter {}

	includedirs
	{
		"include",
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		-- "../../../dependencies/rapidjson",
		-- "../../../dependencies/glm",
		"../../../dependencies/mpack",
		"../../../frameworks/Gaff/include",
		"../../../frameworks/Gleam/include"
	}

	local dependencies =
	{
		"Memory",
		"Gaff",
		"Gleam",
		"Shared",
		"EASTL",
		-- "Resource",
		"mpack"
	}

	dependson(dependencies)
	links(dependencies)

	-- NewDeleteLinkFix()
