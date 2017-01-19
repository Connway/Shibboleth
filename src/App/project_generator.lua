group "Application"

project "App"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/app")
	end

	if _OPTIONS["console_app"] then
		kind "ConsoleApp"
	else
		kind "WindowedApp"
	end

	debugdir "../../workingdir/App"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"../Shared/include",
		"../Memory/include",
		"../../frameworks/Gaff/include",
		"../../dependencies/rapidjson",
		"../../dependencies/EASTL/include"
	}

	dependson
	{
		"Shared", "Gaff",
		"Memory", "EASTL"
	}

	links
	{
		"Shared", "Gaff",
		"Memory", "EASTL"
	}

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	dofile("../../utils/os_conditionals.lua")
