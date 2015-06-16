solution "RenderPipelines"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/renderpipelinesmodule")
	end

	configurations { "Debug", "Release" }
	dofile("../../solution_settings.lua")

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/Gleam/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")
	dofile("../../dependencies/esprit/include_external.lua")
	dofile("../../dependencies/glew/include_external.lua")
	dofile("../../dependencies/assimp/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")
	dofile("../Memory/include_external.lua")

	external "Managers"
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

	external "Game"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

	project "RenderPipelinesModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/renderpipelinesmodule")
		end

		kind "SharedLib"
		language "C++"

		targetname "RenderPipelines"

		files { "**.h", "**.cpp", "**.inl" }

		includedirs
		{
			"../Shared/include",
			"../Memory/include",
			"../Game/include",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/esprit/include",
			"../../dependencies/utf8-cpp",
			"../../dependencies/assimp/include"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"Gleam", "Memory", "Managers",
			"Components", "Game", "assimp",
			"esprit"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"Gleam", "Memory", "Managers",
			"Components", "Game", "assimp",
			"esprit"
		}

		dofile("../../module_suffix.lua")
