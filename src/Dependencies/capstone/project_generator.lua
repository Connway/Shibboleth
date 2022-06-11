Group "Dependencies/Unused By Engine"

project "capstone"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Extra"

	files { "**.c", "**.h" }
	includedirs { "include" }

	defines
	{
		"CAPSTONE_X86_ATT_DISABLE_NO",
		"CAPSTONE_DIET_NO",
		"CAPSTONE_X86_REDUCE_NO",
		"CAPSTONE_HAS_ARM",
		"CAPSTONE_HAS_ARM64",
		"CAPSTONE_HAS_M68K",
		"CAPSTONE_HAS_MIPS",
		"CAPSTONE_HAS_POWERPC",
		"CAPSTONE_HAS_SPARC",
		"CAPSTONE_HAS_SYSZ",
		"CAPSTONE_HAS_X86",
		"CAPSTONE_HAS_XCORE",
		"CAPSTONE_USE_SYS_DYN_MEM"
	}

	SetupConfigMap()

Group "Dependencies"
