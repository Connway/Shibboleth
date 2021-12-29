project "CivetWeb"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "SharedLib"
	language "C++"
	warnings "Extra"

	files { "**.c", "**.h", "**.cpp", "**.inl" }
	includedirs
	{
		"include",
		"src/third_party",
		"../zlib-ng",
		"../sqlite",
		"../lua"
	}

	defines
	{
		"CIVETWEB_CXX_DLL_EXPORTS",
		"CIVETWEB_DLL_EXPORTS",
		"USE_SERVER_STATS",
		"USE_ZLIB",
		"USE_LUA",
		"NO_CACHING",
		"NO_SSL",
		"NO_CGI"
	}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

	filter {}

	local deps =
	{
		"Engine",
		"Memory",
		"Gaff",
		"Lua",
		"sqlite",
		"zlib-ng"
	}

	dependson(deps)
	links(deps)

	SetupConfigMap()
