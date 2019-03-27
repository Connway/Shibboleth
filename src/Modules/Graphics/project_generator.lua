function DoGraphicsModule(renderer)
	local project_name = "Graphics" .. renderer
	local base_dir = GetModulesDirectory("Graphics")

	project(project_name)
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Graphics")
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
			-- base_dir .. "../../Dependencies/rapidjson",
			-- base_dir .. "../../Dependencies/glm",
			-- base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include"
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
			"Gleam",
			-- "Resource",
		}

		dependson(deps)
		links(deps)
end

local GenerateProject = function()
	DoGraphicsModule("Direct3D11")
	-- DoGraphicsModule("Vulkan")
end

local LinkDependencies = function()
	local deps = ModuleDependencies("")
	table.remove(deps) -- remove module name dependency
	table.insert(deps, "Gleam")

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
