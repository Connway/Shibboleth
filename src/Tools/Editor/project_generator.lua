group "Tools"

project "Editor"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/Editor")
	end

	debugdir "../../../workingdir"
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
		"../../../dependencies/nana/include"
	}

	local deps =
	{
		"Shared",
		"Gaff",
		"Memory",
		"EASTL",
		"libjpeg",
		"libpng",
		"zlib-ng",
		"nana"
	}

	dependson(deps)
	links(deps)

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../dependencies/dirent" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../workingdir"
	}
