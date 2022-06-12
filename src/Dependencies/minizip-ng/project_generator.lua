DepProject "minizip-ng"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	excludes { "**bzip.*", "**lzma.*", "**openssl.*", "**libcomp.*", "**crypt_win32.*" }
	defines { "HAVE_ZLIB", "MZ_ZIP_NO_MAIN", "MZ_ZIP_SIGNING" }

	includedirs
	{
		"../zlib-ng",
		"../zstd"
	}

	filter { "system:not macosx" }
		excludes { "**apple.*" }

	filter { "system:not windows" }
		excludes { "**win32.*"  }

	filter { "system:not linux", "system:not macosx" }
		excludes { "**posix.*" }

	filter {}

	SetupConfigMap()
