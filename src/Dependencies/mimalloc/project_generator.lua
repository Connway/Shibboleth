DependencyProject "mimalloc"
	language "C++"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "include" }

	removefiles
	{
		"src/prim/emscripten/*.*",
		"src/prim/osx/*.*",
		"src/prim/unix/*.*",
		"src/prim/wasi/*.*",
		"src/prim/windows/*.*",

		"src/alloc-override.c",
		"src/arena-abandon.c",
		"src/page-queue.c",
		"src/static.c",
		"src/free.c"
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
