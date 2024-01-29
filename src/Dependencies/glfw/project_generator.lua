DependencyProject "GLFW"
	language "C"
	warnings "Extra"

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

	filter { "system:linux", "options:not wayland" }
		removefiles { "src/wl_*.*" }
		defines { "_GLFW_X11" }

	filter { "system:linux", "options:wayland" }
		removefiles { "src/x11_*.*" }
		defines { "_GLFW_WAYLAND" }

	filter { "system:windows" }
		defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()


function GLFWDependencies()
	if os.target() == "windows" then
		return {}
	elseif os.target() == "linux" then
		return {}
	elseif os.target() == "macosx" then
		return {}
	end
end
