project "Gleam"
	if _ACTION then
		location(GetFrameworkLocation())
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/vulkan",
		"../../Dependencies/glm"
	}

	filter { "system:not windows" }
		excludes { "**/*_Direct3D11.*", "*_Direct3D11.*" }

	filter {}

	SetupConfigMap()
