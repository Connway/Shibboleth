function DoMainGraphicsModule()
	local source_dir = GetModulesSourceDirectory("Graphics")
	local base_dir = GetModulesDirectory("Graphics")

	GenProject "GraphicsBase"
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		excludes
		{
			source_dir .. "include/Shibboleth_RenderManager.h",
			source_dir .. "Shibboleth_RenderManager.cpp",
			source_dir .. "Shibboleth_GraphicsModule.cpp"
		}

		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			source_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
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
	local source_dir = GetModulesSourceDirectory("Graphics")
	local base_dir = GetModulesDirectory("Graphics")
	local project_name = "Graphics" .. renderer

	GenProject(project_name)
		location(GetModulesLocation())

		language "C++"

		files
		{
			source_dir .. "include/Shibboleth_RenderManager.h",
			source_dir .. "Shibboleth_RenderManager.cpp",
			source_dir .. "Shibboleth_GraphicsModule.cpp"
		}

		defines { "SHIB_STATIC" }

		SetupConfigMap()

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
		elseif renderer == "Direct3D12" then
			defines { "USE_D3D12" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		flags { "FatalWarnings" }

		includedirs
		{
			source_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
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

		local deps =
		{
			"GraphicsBase"
		}

		dependson(deps)


	GenProject(project_name .. "Module", "SharedLib")
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "Shibboleth_GraphicsModule.cpp" }

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
			links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks(project_name, "Graphics")
		SetupConfigMap()
		ModuleCopy()

		deps =
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
			"GLFW"
		}

		dependson(deps)
		links(deps)

		NewDeleteLinkFix()
end

local GenerateProject = function()
	DoMainGraphicsModule()
	DoGraphicsModule("Direct3D11")
	-- DoGraphicsModule("Direct3D12")

	
	-- DoGraphicsModule("Vulkan")
end

local LinkDependencies = function()
	local deps = ModuleDependencies("")
	table.remove(deps) -- remove module name dependency
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Gleam")
	table.insert(deps, "ECS")
	table.insert(deps, "assimp")
	table.insert(deps, "mpack")
	table.insert(deps, "minizip-ng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")
	table.insert(deps, "GLFW")

	dependson(deps)
	links(deps)

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
