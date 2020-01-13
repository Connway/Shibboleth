project "CivetWeb"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
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
		"USE_SERVER_STATS",
		"USE_ZLIB",
		"USE_LUA",
		"NO_CACHING",
		"NO_SSL",
		"NO_CGI"
	}

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()
