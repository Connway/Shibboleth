project "Engine"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows", "action:not vs*", "configurations:*Clang" }
		includedirs { "../../Dependencies/dirent" }

	filter { "system:windows", "action:vs*" }
		includedirs { "../../Dependencies/dirent" }

	filter {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glm"
	}
