project "MainLoopModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	targetname "MainLoop"

	files { "**.h", "**.cpp", "**.inl" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../Dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../../Engine/Shared/include",
		"../../Engine/Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/EASTL/include"

		, "../Scripting/include",
		"../Resource/include",
		"../../Dependencies/angelscript/angelscript/include",
		"../../Dependencies/angelscript/add_on/scriptbuilder",
		"../Entity/include",
		"../../Frameworks/Gleam/include",
		"../../Dependencies/glm",
		"../../Dependencies/rapidjson"
	}

	local deps = {
		"Shared",
		"Gaff",
		"Memory",
		"EASTL"

		, "Scripting", "Resource", "angelscript", "Entity", "Gleam", "mpack"
	}

	dependson(deps)
	links(deps)

	postbuildcommands
	{
		"{MKDIR} ../../../../workingdir/bin",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../workingdir/bin"
	}
