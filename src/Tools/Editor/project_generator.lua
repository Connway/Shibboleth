group "Tools"

project "Editor"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/Editor")
	end

	debugdir "../../../workingdir/App"
	kind "WindowedApp"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../Shared/include",
		"../../Memory/include",
		"../../../frameworks/Gaff/include",
		"../../../dependencies/rapidjson",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/libuv/include"
	}

	dependson
	{
		"Shared", "Gaff",
		"Memory", "EASTL",
		"libuv"
	}

	links
	{
		"Shared", "Gaff",
		"Memory", "EASTL",
		"libuv"
	}

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../dependencies/dirent" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	dofile("../../../utils/os_conditionals.lua")

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../workingdir/App"
	}
