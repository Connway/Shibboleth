project "Esprit"
	if _ACTION then
		location(GetFrameworkLocation())
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glm"
	}

	SetupConfigMap()
