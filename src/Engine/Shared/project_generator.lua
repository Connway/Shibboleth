project "Shared"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
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
