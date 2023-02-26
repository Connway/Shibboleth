if _ACTION == "gmake2" then
	return
end

project "glm"
	location(GetDependenciesLocation())

	kind "None"
	-- language "C++"
	-- warnings "Extra"

	files
	{
		"**.hpp",
		"**.h",
		"**.inl",
		"**.natvis"
	}

	SetupConfigMap()
