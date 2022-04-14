if _ACTION == "gmake2" then
	return
end

group "Dependencies/Unused By Engine"

project "acl"
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

group "Dependencies"
