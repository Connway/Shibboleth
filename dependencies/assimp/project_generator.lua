project "assimp"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

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

	excludes
	{
		"contrib/zip/test/*.*"
	}

	includedirs
	{
		".",
		"contrib/openddlparser/include",
		"contrib/irrXML",
		"include",
		"../rapidjson",
		"../minizip",
		"../zlib"
	}

	defines
	{
		"ASSIMP_BUILD_BOOST_WORKAROUND",
		"ASSIMP_BUILD_NO_OWN_ZLIB",
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"OPENDDL_STATIC_LIBARY"
	}

	rtti "On"

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

	filter { "action:vs*" }
		buildoptions { "/bigobj" }

	filter {}
