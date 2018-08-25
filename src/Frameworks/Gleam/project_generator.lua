project "Gleam"
	if _ACTION then
		location(GetFrameworkLocation())
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "action:not vs*", "configurations:*Clang" }
		buildoptions { "-Wno-varargs" }

	filter {}

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/vulkan",
		"../../Dependencies/glm"
	}
