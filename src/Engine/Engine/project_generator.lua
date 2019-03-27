project "Engine"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/mpack",
		"../../Dependencies/glm"
	}

	filter { "configurations:Static_*" }
		StaticHeaderGen()

	filter {}

	SetupConfigMap()
