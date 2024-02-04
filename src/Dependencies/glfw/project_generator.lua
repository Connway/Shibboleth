DependencyProject("GLFW", "SharedLib")
	language "C"
	warnings "Extra"

	defines { "_GLFW_BUILD_DLL" }

	files { "**.c", "**.h" }

	filter { "system:not linux", "system:not macosx" }
		removefiles { "src/posix_*.*", "src/glx_context.c" }

	filter { "system:not macosx" }
		removefiles { "src/cocoa_*.*" }

	filter { "system:not linux" }
		removefiles { "src/linux_*.*", "src/wl_*.*", "src/x11_*.*", "src/xkb_*.*" }

	filter { "system:not windows" }
		removefiles { "src/win32_*.*", "src/wgl_*.*" }

	filter { "system:macosx" }
		defines { "_GLFW_COCOA" }
		files { "**.m" }
		links { "Cocoa.framework", "IOKit.framework" }

	filter { "system:linux", "options:not wayland" }
		removefiles { "src/wl_*.*" }
		defines { "_GLFW_X11" }
		-- links {}

	filter { "system:linux", "options:wayland" }
		removefiles { "src/x11_*.*" }
		defines { "_GLFW_WAYLAND" }
		-- links {}

	filter { "system:windows" }
		defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }
		-- links {}

	filter {}

	SetupConfigMap()
