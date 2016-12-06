project "angelscript"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"
	warnings "Default"

	files
	{
		"angelscript/**.cpp",
		"angelscript/**.h",
		"angelscript/**.inl",
	}

	includedirs { "angelscript/include" }
	removefiles { "**/projects/**.*"}

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
