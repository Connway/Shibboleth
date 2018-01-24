project "Gleam"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	-- configuration "windows"
	-- 	includedirs { "../../dependencies/dirent" }
	-- 	defines { "_CRT_SECURE_NO_WARNINGS" }

	defines { "GLEW_STATIC" }

	files { "**.h", "**.cpp" }

	removefiles { "disable/**.*" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../../dependencies/glew/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/vulkan",
		"../../dependencies/glm"
	}
