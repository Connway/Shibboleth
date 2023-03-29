local deps =
{
	"GraphicsBase",
	"MainLoop",
	"Resource",
	"Gleam",
	"ECS",
	"assimp",
	"mpack",
	"minizip-ng",
	"zlib-ng",
	"libpng",
	"libtiff",
	"GLFW",
	"DevDebug"
}

local source_dir = GetModulesSourceDirectory("Graphics")
local base_dir = GetModulesDirectory("Graphics")

function DoMainGraphicsModule()
	ModuleProject("GraphicsBase", "Graphics")
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		-- Possibly doesn't support tokens?
		removefiles
		{
			source_dir .. "include/Shibboleth_RenderManager.h",
			source_dir .. "Shibboleth_RenderManager.cpp",
			source_dir .. "Shibboleth_GraphicsModule.cpp"
		}

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
			source_dir .. "../../Modules/ECS/include",
			source_dir .. "../../Modules/MainLoop/include"
		}
end

function DoGraphicsModule(renderer)
	local project_name = "Graphics" .. renderer

	ModuleProject(project_name, "Graphics")
		language "C++"

		files
		{
			source_dir .. "include/Shibboleth_RenderManager.h",
			source_dir .. "Shibboleth_RenderManager.cpp",
			source_dir .. "Shibboleth_GraphicsModule.cpp"
		}

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
		elseif renderer == "Direct3D12" then
			defines { "USE_D3D12" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/ECS/include",
			source_dir .. "../../Modules/MainLoop/include"
		}

	ModuleProject(project_name .. "Module", "Graphics")
		language "C++"

		files { source_dir .. "Shibboleth_GraphicsModule.cpp" }

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
			links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		elseif renderer == "Direct3D12" then
			defines { "USE_D3D12" }
			-- links { "d3d12", "D3dcompiler", "dxgi", "dxguid" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		dependson(deps)
		links(deps)
end

local GenerateProject = function()
	DoMainGraphicsModule()
	DoGraphicsModule("Direct3D11")
	-- DoGraphicsModule("Direct3D12")
	-- DoGraphicsModule("Vulkan")
end

local LinkDependencies = function()
	local link_deps = ModuleDependencies("")
	table.remove(link_deps) -- remove module name dependency
	table.insertflat(link_deps, deps)

	dependson(link_deps)
	links(link_deps)

	filter { "configurations:Static_*_D3D11*" }
		dependson("GraphicsDirect3D11")
		links { "GraphicsDirect3D11", "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		defines { "USE_D3D11" }

	filter { "configurations:Static_*_D3D12*" }
		dependson("GraphicsDirect3D12")
		links { "GraphicsDirect3D12", "d3d12", "D3dcompiler", "dxgi", "dxguid" }
		defines { "USE_D3D12" }

	filter { "configurations:Static_*_Vulkan*" }
		dependson("GraphicsVulkan")
		links("GraphicsVulkan")
		defines { "USE_VULKAN" }

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
