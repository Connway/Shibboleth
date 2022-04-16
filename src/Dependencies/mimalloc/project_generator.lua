project "mimalloc"
	location(GetDependenciesLocation())

	kind "StaticLib"
	-- kind "SharedLib"
	language "C++"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "include" }

	removefiles
	{
		"src/alloc-override*.c",
		"src/page-queue.c",
		"src/static.c"
	}

	-- defines
	-- {
	-- 	"MI_SHARED_LIB",
	-- 	"MI_SHARED_LIB_EXPORT",
	-- 	"MI_MALLOC_OVERRIDE"
	-- }

	filter { "configurations:*Debug*" }
		defines { "MI_DEBUG=3" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
