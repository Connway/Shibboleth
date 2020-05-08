project "imgui"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"
	warnings "Extra"

	files { "**.c", "**.h", "**.cpp" }

	includedirs
	{
		"../../Frameworks/Gaff/include"
	}

	SetupConfigMap()
