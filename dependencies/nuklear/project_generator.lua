project "nuklear"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"
	flags { "FatalWarnings" }

	files { "**.cpp", "**.h" }

	includedirs
	{
		"../../frameworks/Gaff/include",
		"../../src/Shared/include"
	}
