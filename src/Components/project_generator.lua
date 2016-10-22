group "Components Module"

project "Components"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/components")
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	excludes { "Shibboleth_ComponentsModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Managers/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/assimp/include",
		"../../dependencies/bullet"
	}

	dofile("../../utils/os_conditionals.lua")

project "ComponentsModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/components")
	end

	kind "SharedLib"
	language "C++"

	targetname "ShibbolethComponents"

	files { "Shibboleth_ComponentsModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Managers/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/assimp/include",
		"../../dependencies/bullet"
	}

	dependson
	{
		"Shared", "Gaff", "libjpeg",
		"libpng", "libtiff",
		"OtterUI", "ResIL", "ResILU",
		"zlib", "Gleam", "Game",
		"esprit", "Components",
		"Memory", "LuaJIT",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath"
	}

	links
	{
		"Shared", "Gaff", "libjpeg",
		"libpng", "libtiff",
		"OtterUI", "ResIL", "ResILU",
		"zlib", "Gleam", "Game",
		"esprit", "Components",
		"Memory",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath"
	}

	dofile("../../utils/os_conditionals.lua")
