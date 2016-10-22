group "Managers Module"

project "Managers"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/managers")
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }
	excludes { "Shibboleth_ManagersModule.cpp" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Components/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/assimp/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/bullet",
		"../../dependencies/imgui",
		"../../dependencies/nuklear"
	}

	dofile("../../utils/os_conditionals.lua")

project "ManagersModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/managers")
	end

	dofile("../../utils/module_suffix.lua")

	kind "SharedLib"
	language "C++"

	flags { "FatalWarnings" }

	targetname "ShibbolethManagers"

	files { "Shibboleth_ManagersModule.cpp" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Components/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/bullet",
		"../../dependencies/nuklear"
	}

	dependson
	{
		"Shared", "Gaff",
		"OtterUI", "Game",
		"Gleam", "Managers",
		"Memory", "LuaJIT",
		"Boxer", "Components",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath", "imgui",
		"nuklear"
	}

	links
	{
		"Shared", "Gaff",
		"OtterUI", "Game",
		"Gleam", "Managers",
		"Memory", "LuaJIT",
		"Components",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath", "imgui",
		"nuklear"
	}

	dofile("../../utils/os_conditionals.lua")
