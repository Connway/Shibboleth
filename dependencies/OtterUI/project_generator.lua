project "OtterUI"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp" }

	includedirs { "inc", "src" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}
