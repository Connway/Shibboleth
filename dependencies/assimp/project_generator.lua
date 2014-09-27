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

	includedirs
	{
		"code/BoostWorkaround",
		"../minizip",
		"../zlib"
	}

	defines { "ASSIMP_BUILD_BOOST_WORKAROUND", "ASSIMP_BUILD_NO_OWN_ZLIB" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }
		removeflags { "NoRTTI" }

	filter {}
