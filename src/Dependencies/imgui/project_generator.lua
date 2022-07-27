DepProject "imgui"
	language "C++"
	warnings "Extra"

	files { "**.c", "**.h", "**.cpp" }

	IncludeDirs
	{
		"../../Frameworks/Gaff/include",
		"."
	}
	
	removefiles { "misc/freetype/*.*" }

	SetupConfigMap()
