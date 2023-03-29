DependencyProject "imgui"
	language "C++"
	warnings "Extra"

	files { "**.c", "**.h", "**.cpp", "**.natvis" }

	includedirs
	{
		"../../Frameworks/Gaff/include",
		"."
	}

	removefiles { "misc/freetype/*.*" }

	SetupConfigMap()
