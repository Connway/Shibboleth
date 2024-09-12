function AngelScriptDefines()
	filter { "configurations:*Debug*" }
		defines { "AS_DEBUG" }

	filter {}

	defines
	{
		"AS_NO_EXCEPTIONS"
	}
end

DependencyProject "AngelScript"
	language "C++"
	warnings "Extra"

	files
	{
		"angelscript/**.h",
		"angelscript/**.cpp"
	}

	includedirs
	{
		"angelscript/include"
	}

	AngelScriptDefines()

	filter { "system:macosx", "platforms:arm64" }
		files { "angelscript/source/as_callfunc_arm64_xcode.asm"}

	filter { "system:windows", "action:vs*", "platforms:arm64" }
		files { "angelscript/source/as_callfunc_arm64_msvc.asm"}


	filter { "system:windows", "action:vs*", "platforms:x64" }
		files { "angelscript/source/as_callfunc_x64_msvc_asm.asm"}

	--filter { "system:windows" }
	--	defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()
