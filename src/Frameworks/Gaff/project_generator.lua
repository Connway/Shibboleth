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

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../Dependencies/dirent" }
		defines { "_CRT_SECURE_NO_WARNINGS" }
		excludes { "**/*_Linux.*", "*_Linux.*" }

	filter {}
