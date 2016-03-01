	project "Boxer"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/dependencies")
		end

		configurations { "Debug", "Release" }
		dofile("../../utils/config_map.lua")

		kind "StaticLib"
		language "C++"
		warnings "Default"

		includedirs { "include" }
		files { "**.h" }

		filter { "system:windows" }
			defines { "_CRT_SECURE_NO_WARNINGS" }
			files { "src/boxer_win.cpp" }

		filter { "system:linux" }
			files { "src/boxer_linux.cpp" }

		filter { "system:macosx" }
			files { "src/boxer_osx.mm" }

		filter {}
