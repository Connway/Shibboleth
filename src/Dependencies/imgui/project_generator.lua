DepProject "imgui"
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
