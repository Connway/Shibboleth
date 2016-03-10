group "Application"

project "App"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/app")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	dofile("../../utils/module_suffix.lua")

	if _OPTIONS["console_app"] then
		kind "ConsoleApp"
	else
		kind "WindowedApp"
	end

	debugdir "../../workingdir/App"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

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
		"Shared", "Gaff", "jansson", "Memory",
		"Boxer"
	}

	links
	{
		"Shared", "Gaff", "jansson", "Memory",
		"Boxer"
	}

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	dofile("../../utils/os_conditionals.lua")
