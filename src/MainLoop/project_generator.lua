group "Main Loop Module"

project "MainLoopModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/states")
	end

	kind "SharedLib"
	language "C++"

	targetname "MainLoop"

	files { "**.h", "**.cpp", "**.inl" }

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
		"../Components/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson"
	}

	dependson
	{
		"Shared", "Gaff",
		"libjpeg", "libpng",
		"libtiff", "OtterUI",
		"ResIL", "ResILU",
		"zlib", "Gleam",
		"Game", "esprit",
		"Components",
		"States", "Memory",
		"LuaJIT",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath",
		"assimp", "minizip"
	}

	links
	{
		"Shared", "Gaff",
		"libjpeg", "libpng",
		"libtiff", "OtterUI",
		"ResIL", "ResILU",
		"zlib", "Gleam",
		"Game", "esprit",
		"Components",
		"Memory","LuaJIT",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath",
		"assimp", "minizip"
	}

	dofile("../../utils/os_conditionals.lua")
