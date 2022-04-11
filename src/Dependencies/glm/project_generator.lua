project "glm"
	location(GetDependenciesLocation())

	kind "None"
	-- language "C++"
	-- warnings "Extra"

	files
	{
		"**.hpp",
		"**.h",
		"**.inl"
	}

	SetupConfigMap()
