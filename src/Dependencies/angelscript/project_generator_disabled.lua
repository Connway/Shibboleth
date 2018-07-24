project "angelscript"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"
	warnings "Default"

	files
	{
		"add_on/scriptbuilder/*.*",
		"add_on/scriptarray/*.*",
		"angelscript/**.cpp",
		"angelscript/**.h",
		"angelscript/**.inl",
	}

	includedirs { "angelscript/include" }

	filter { "system:windows" }
		files { "**/as_callfunc_x64_msvc_asm.asm" }
		removefiles
		{
			"**_xenon.*",
			"**_mingw.*",
			"**_gcc.*",
			"**_ppc*.*",
			"**_sh4.*",
			"**_mips.*",
			"**_arm.*",
		}

	filter { "system:windows", "architecture:not x64" }
		excludes { "**/as_callfunc_x64_msvc_asm.asm" }

	filter { "architecture:x64" }
		excludes { "**_x86.*" }

	filter {}
