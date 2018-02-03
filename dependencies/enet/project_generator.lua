project "enet"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "include" }

	filter { "configurations:Debug* or Optimized_Debug*" }
		defines { "ENET_DEBUG" }

	filter { "system:windows" }
		defines { "_WINSOCK_DEPRECATED_NO_WARNINGS" }

	filter { "system:not windows" }
		defines { "HAS_SOCKLEN_T" }

	filter {}
