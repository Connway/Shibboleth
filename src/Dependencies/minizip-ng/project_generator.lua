DependencyProject "minizip-ng"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	removefiles { "**bzip.*", "**lzma.*", "**openssl.*", "**libcomp.*", "**crypt_winvista.*", "**crypt_winxp.*" }
	defines { "HAVE_ZLIB", "MZ_ZIP_NO_MAIN", "MZ_ZIP_SIGNING" }

	includedirs
	{
		"../zlib-ng",
		"../zstd"
	}

	filter { "system:not macosx" }
		removefiles { "**apple.*" }

	filter { "system:not windows" }
		removefiles { "**win32.*"  }

	filter { "system:not linux", "system:not macosx" }
		removefiles { "**posix.*" }

	filter {}

	SetupConfigMap()
