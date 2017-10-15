project "glew"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	defines { "GLEW_STATIC" }

	files { "**.h", "**.c" }
	includedirs { "include" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}
