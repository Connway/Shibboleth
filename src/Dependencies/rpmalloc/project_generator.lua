project "rpmalloc"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"
	warnings "Default"

	files { "**.cpp", "**.h", "**.inl", "**.hpp" }

	SetupConfigMap()
