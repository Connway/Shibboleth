project "libuv"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "include", "src" }

	filter { "system:windows" }
		defines
		{
			"_CRT_SECURE_NO_DEPRECATE", "_CRT_NONSTDC_NO_DEPRECATE",
			"_WIN32_WINNT=0x0600", "_GNU_SOURCE"
		}

		removefiles { "src/unix/*.*" }

	filter {}
