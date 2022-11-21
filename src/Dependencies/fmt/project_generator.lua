if _ACTION == "gmake2" then
	return
end

project "fmt"
	location(GetDependenciesLocation())

	kind "None"
	-- language "C++"
	-- warnings "Extra"

	files
	{
		"**.hpp",
		"**.h",
		"**.inl",
		"**.cc"
	}

	SetupConfigMap()
