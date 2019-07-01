function DoMainGraphicsModule()
	local base_dir = GetModulesDirectory("Graphics")

	project("GraphicsBase")
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl", base_dir .. "**.lua" }

		excludes
		{
			base_dir .. "include/Shibboleth_RenderManager.h",
			base_dir .. "Shibboleth_RenderManager.cpp",
			base_dir .. "Shibboleth_GraphicsModule.cpp"
		}

		ModuleGen("Graphics")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/assimp/include",
			-- base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/ECS/include"
		}
end

function DoGraphicsModule(renderer)
	local project_name = "Graphics" .. renderer
	local base_dir = GetModulesDirectory("Graphics")

	project(project_name)
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files
		{
			base_dir .. "include/Shibboleth_RenderManager.h",
			base_dir .. "Shibboleth_RenderManager.cpp",
			base_dir .. "Shibboleth_GraphicsModule.cpp"
		}

		defines { "SHIB_STATIC" }

		SetupConfigMap()

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Modules/Resource/include"
		}


	project(project_name .. "Module")
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_GraphicsModule.cpp" }

		ModuleCopy()

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
			links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks(project_name, "Graphics")
		SetupConfigMap()

		local deps =
		{
			"GraphicsBase",
			"Gleam",
			"Resource",
			"ECS",
			"assimp",
			"mpack",
			"minizip",
			"zlib-ng"
		}

		dependson(deps)
		links(deps)

		NewDeleteLinkFix()
end

local GenerateProject = function()
	DoMainGraphicsModule()
	DoGraphicsModule("Direct3D11")
	-- DoGraphicsModule("Vulkan")
end

local LinkDependencies = function()
	local deps = ModuleDependencies("")
	table.remove(deps) -- remove module name dependency
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "Gleam")
	table.insert(deps, "Resource")
	table.insert(deps, "ECS")
	table.insert(deps, "assimp")
	table.insert(deps, "mpack")
	table.insert(deps, "minizip")
	table.insert(deps, "zlib-ng")

	dependson(deps)
	links(deps)
end

local FilterDependencies = function()
	filter { "configurations:Static_*_D3D11" }
		dependson("GraphicsDirect3D11")
		links { "GraphicsDirect3D11", "d3d11", "D3dcompiler", "dxgi", "dxguid" }

	-- filter { "configurations:Static_*_Vulkan" }
	-- 	dependson("GraphicsVulkan")
	-- 	links("GraphicsVulkan")

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies, FilterDependencies = FilterDependencies }
