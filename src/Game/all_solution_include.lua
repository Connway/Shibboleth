group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/libjpeg/include_external.lua")
	dofile("../../dependencies/libpng/include_external.lua")
	dofile("../../dependencies/libtiff/include_external.lua")
	dofile("../../dependencies/OtterUI/include_external.lua")
	dofile("../../dependencies/ResIL/include_external.lua")
	dofile("../../dependencies/zlib/include_external.lua")
	dofile("../../dependencies/Gleam/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")
	dofile("../../dependencies/LuaJIT-2.0.3/include_external.lua")
	dofile("../../dependencies/assimp/include_external.lua")
	dofile("../../dependencies/minizip/include_external.lua")
	dofile("../../dependencies/esprit/include_external.lua")

group "Common"
	dofile("../Shared/include_external.lua")

group "GameModules"
	external "Game"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

	external "Components"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

	external "Managers"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

	external "Messages"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

	external "States"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

	external "ManagersModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

	external "StatesModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

	external "ComponentsModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"
