project "Editor"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Engine/include",
		"../Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/EASTL/include"
	}

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}
