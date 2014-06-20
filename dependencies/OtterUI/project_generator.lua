project "OtterUI"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configuration "windows"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp" }

	includedirs { "inc", "src" }
