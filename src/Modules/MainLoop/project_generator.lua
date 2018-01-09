group "Modules/Main Loop"

project "MainLoopModule"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/mainloop")
	end

	kind "SharedLib"
	language "C++"

	targetname "MainLoop"

	files { "**.h", "**.cpp", "**.inl" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../../Shared/include",
		"../../Memory/include",
		"../../../frameworks/Gaff/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/libuv/include"

		, "../Scripting/include",
		"../Resource/include",
		"../../../dependencies/angelscript/angelscript/include",
		"../../../dependencies/angelscript/add_on/scriptbuilder",
		"../Entity/include",
		"../../../frameworks/Gleam/include",
		"../../../dependencies/glm",
		"../../../dependencies/rapidjson"
	}

	dependson
	{
		"Shared",
		"Gaff",
		"Memory",
		"EASTL",
		"libuv"

		, "Scripting", "Resource", "angelscript", "Entity", "Gleam"
	}

	links
	{
		"Shared",
		"Gaff",
		"Memory",
		"EASTL",
		"libuv"

		, "Scripting", "Resource", "angelscript", "Entity", "Gleam"
	}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../workingdir/App/bin"
	}
