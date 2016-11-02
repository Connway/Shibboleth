project "libjpeg"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files
	{
		"**.h",
		"**.c",
		"**.cfg",
		"**.mac",
		"**.vc"
	}

	excludes
	{
		"cd*.c",
		"cd*.h",
		"rd*.c",
		"cjpeg.c",
		"ckconfig.c",
		"djpeg.c",
		"example.c",
		"jmemansi.c",
		"jmemdos.c",
		"jmemmac.c",
		"jmemname.c",

		"wrbmp.c",
		"wrgif.c",
		"wrjpgcom.c",
		"wrppm.c",
		"wrrle.c",
		"wrtarga.c",

		"jpegtran.c"
	}

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "action:gmake", "toolset:gcc or clang" }
		buildoptions { "-fPIC" }

	filter {}
