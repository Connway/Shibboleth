if _ACTION == "gmake2" then
	return
end

Group "Dependencies/Unused by Engine"

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

Group "Dependencies"
