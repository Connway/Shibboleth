project "assimp"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"
	warnings "Default"

	files
	{
		"**.cpp",
		"**.h",
		"**.cc",
		"**.hpp",
		"**.inl",
		"**.c"
	}

	includedirs
	{
		"code/BoostWorkaround",
		"contrib/openddlparser/include",
		"include",
		"../minizip",
		"../zlib"
	}

	defines { "ASSIMP_BUILD_BOOST_WORKAROUND", "ASSIMP_BUILD_NO_OWN_ZLIB", "ASSIMP_BUILD_NO_C4D_IMPORTER" }
	rtti "On"

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

	filter {}

	configuration "vs2015"
		buildoptions { "/bigobj" }

	configuration {}
