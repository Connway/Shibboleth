group "Modules/Resource"

project "Resource"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/resource")
	end

	kind "StaticLib"
	language "C++"
	rtti "On"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ResourceModule.cpp" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../Game/include",
		"../../dependencies/EASTL/include",
		-- "../../dependencies/rapidjson",
		"../../frameworks/Gaff/include"
	}

	filter { "action:vs*" }
		buildoptions { "/bigobj" }

	filter {}

-- project "ResourceModule"
-- 	if _ACTION then
-- 		location ("../../project/" .. _ACTION .. "/resource")
-- 	end

-- 	kind "SharedLib"
-- 	language "C++"

-- 	files { "Shibboleth_ResourceModule.cpp" }

-- 	ModuleGen("Resource")
-- 	ModuleCopy()

-- 	includedirs
-- 	{
-- 		"include",
-- 		"../Memory/include",
-- 		"../Shared/include",
-- 		"../Game/include",
-- 		"../../dependencies/EASTL/include",
-- 		"../../dependencies/rapidjson",
-- 		"../../frameworks/Gaff/include"
-- 	}

-- 	dependson
-- 	{
-- 		"Resource", "Memory",
-- 		"Gaff", "Shared",
-- 		"EASTL", "Game"
-- 	}

-- 	links
-- 	{
-- 		"Resource", "Memory",
-- 		"Gaff", "Shared",
-- 		"EASTL", "Game"
-- 	}
