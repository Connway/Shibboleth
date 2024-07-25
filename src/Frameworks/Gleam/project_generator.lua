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

function GleamRendererLinksArray()
	if os.target() == "windows" and (_OPTIONS["renderer"] == nil or _OPTIONS["renderer"] == "d3d11") then
		return { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
	elseif os.target() == "windows" and _OPTIONS["renderer"] == "d3d12" then
		return { "d3d12", "D3dcompiler", "dxgi", "dxguid" }
	elseif os.target() == "linux" then
		-- Vulkan
		return {}
	elseif os.target() == "macosx" then
		return {}
	end

	return {}
end

function GleamRendererLinks()
	links(GleamRendererLinksArray())
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
