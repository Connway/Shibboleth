group "Application"

project "App"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/app")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	if _OPTIONS["console_app"] then
		kind "ConsoleApp"
	else
		kind "WindowedApp"
	end

	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"../Shared/include",
		"../Memory/include",
		"../../dependencies/jansson",
		"../../dependencies/Gaff/include",
		"../../dependencies/utf8-cpp"
	}

	dependson
	{
		"Shared", "Gaff", "jansson", "Memory"
	}

	links
	{
		"Shared", "Gaff", "jansson", "Memory"
	}