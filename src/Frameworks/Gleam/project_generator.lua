newoption
{
	trigger = "renderer",
	description = "Which renderer the Graphics Module will use when built.",
	allowed =
	{
		{ "d3d11", "Direct3D 11" },
		{ "d3d12", "Direct3D 12" },
		{ "vulkan", "Vulkan" },
		{ "metal", "Metal" }
	}
}

function GleamRendererDefines()
	filter { "system:windows", "options:not renderer or renderer=d3d11" }
		defines { "GLEAM_USE_D3D11" }

	filter { "system:windows", "options:renderer=d3d12" }
		defines { "GLEAM_USE_D3D12" }

	filter { "system:windows", "options:renderer=vulkan" }
		defines { "GLEAM_USE_VULKAN" }

	filter { "system:linux" }
		defines { "GLEAM_USE_VULKAN" }

	filter { "system:macosx" }
		defines { "GLEAM_USE_METAL" }

	filter {}
end

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
