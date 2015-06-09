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
	dofile("../../dependencies/glew/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")
	dofile("../Memory/include_external.lua")

	external "Managers"
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

		files { "**.h", "**.cpp", "**.inl" }

		includedirs
		{
			"../Shared/include",
			"../Memory/include",
			"../Game/include",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/utf8-cpp"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"Gleam", "Memory", "Managers"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"Gleam", "Memory", "Managers"
		}

		filter {}