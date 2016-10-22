project "jemalloc"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	defines { "JEMALLOC_EXPORT=" }

	files { "**.h", "**.c" }
	includedirs { "include" }

	filter { "configurations:Debug* or Optimized_Debug*"}
		defines { "JEMALLOC_DEBUG" }

	filter { "system:not macos"}
		excludes { "src/zone.c" }

	filter { "action:vs*" }
		includedirs { "include/msvc_compat" }

	filter { "system:windows" }
		excludes { "src/valgrind.c" }

	filter { "action:vs*" }
		buildoptions
		{
			"/wd4090",
			"/wd4146",
			"/wd4267",
			"/wd4334"
		}

	filter {}
