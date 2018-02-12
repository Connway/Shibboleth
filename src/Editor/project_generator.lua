group "Common"

project "Editor"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/Editor")
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../../frameworks/Gaff/include",
		"../../dependencies/EASTL/include"
	}

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}
