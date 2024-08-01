DependencyProject "Luau"
	language "C++"
	warnings "Extra"

	files { "**.h", "**.cpp" }

	defines { "_CRT_SECURE_NO_WARNINGS" }

	includedirs
	{
		"Compiler/include",
		"Common/include",
		"Ast/include",
		"VM/include"
	}

	SetupConfigMap()
