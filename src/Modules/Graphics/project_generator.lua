local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Graphics")
	local base_dir = GetModulesDirectory("Graphics")

	ModuleProject "Graphics"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }
		removefiles { source_dir .. "Shibboleth_GraphicsRenderer.cpp" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/assimp/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Entity/include",
			source_dir .. "../../Modules/MainLoop/include"
		}

		GleamRendererDefines()

	ModuleProject "GraphicsModule"
		language "C++"

		files { source_dir .. "Shibboleth_GraphicsModule.cpp" }

		GleamRendererDefines()

		filter { "system:windows", "options:not renderer or renderer=d3d11" }
			links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }

		filter { "system:windows", "options:renderer=d3d12" }
			links { "d3d12", "D3dcompiler", "dxgi", "dxguid" }

		filter { "system:windows", "options:renderer=vulkan" }
			--dependson {}
			--links {}

		filter { "system:linux" }
			-- Vulkan
			--dependson {}
			--links {}

		filter { "system:macosx" }
			-- Metal
			--dependson {}
			--links {}

		filter {}


		local deps =
		{
			"GraphicsBase",
			"MainLoop",
			"Resource",
			"Gleam",
			"Entity",
			"assimp",
			"mpack",
			"minizip-ng",
			"zlib-ng",
			"libpng",
			"libtiff",
			"GLFW",
			"DevDebug"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Graphics")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
