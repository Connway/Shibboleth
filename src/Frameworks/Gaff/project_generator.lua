project "Gaff"
	if _ACTION then
		location(GetFrameworkLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/mpack"
	}

	flags { "FatalWarnings" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()
