project "libjpeg"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

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

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}

	filter { "action:gmake" }
		buildoptions { "-fPIC" }

	filter {}
