group "Dependencies/Unused By Engine"

project "GLFW"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }

	filter { "system:windows" }
		removefiles { "src/cocoa_*.*", "src/linux_*.*", "src/posix_*.*", "src/wl_*.*", "src/x11_*.*", "src/xkb_*.*", "src/glx_context.c" }
		defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()

group "Dependencies"
