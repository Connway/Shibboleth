project "glm"
	-- if _ACTION then
	-- 	location(GetDependenciesLocation())
	-- end

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
