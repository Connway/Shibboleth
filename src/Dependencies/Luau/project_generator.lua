DependencyProject "Luau"
	language "C++"
	warnings "Extra"

	files { "**.h", "**.cpp" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}

	includedirs
	{
		"Compiler/include",
		"CodeGen/include",
		"Common/include",
		"Ast/include",
		"VM/include",
		"VM/src"
	}

	SetupConfigMap()
