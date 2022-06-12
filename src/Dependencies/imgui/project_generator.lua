DepProject "imgui"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C++"
	warnings "Extra"

	files { "**.c", "**.h", "**.cpp" }

	includedirs
	{
		"../../Frameworks/Gaff/include",
		"."
	}
	
	removefiles { "misc/freetype/*.*" }

	SetupConfigMap()
