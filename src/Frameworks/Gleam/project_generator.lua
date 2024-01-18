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
		--"../../Dependencies/vulkan"
	}

	GleamRendererDefines()
