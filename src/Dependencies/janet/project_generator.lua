project "Janet"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Extra"

	includedirs { "include", "conf" }
	files { "**.c", "**.h" }

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"JANET_BOOTSTRAP"
	}

	SetupConfigMap()
