FrameworkProject "Gleam"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glfw/include",
		"../../Dependencies/glm",
		"../../Dependencies/vulkan"
	}

	filter { "system:not windows" }
		removefiles { "**/*_Direct3D11.*", "*_Direct3D11.*" }

	filter {}
