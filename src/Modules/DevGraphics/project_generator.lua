local GenerateProject = function()
	local base_dir = GetModulesDirectory("DevGraphics")

	project "DevGraphics"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()
		ModuleGen("DevGraphics")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/CivetWeb/include",
			base_dir .. "../../Dependencies/nvenc",
			base_dir .. "../../Modules/DevWebServer/include",
			base_dir .. "../../Modules/ECS/include",
			base_dir .. "../../Modules/Graphics/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include"
		}

	project "DevGraphicsModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_DevGraphicsModule.cpp" }

		ModuleCopy("DevModules")

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("DevGraphics")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"Gleam",

			"DevWebServer",
			--"MainLoop",
			"Resource",
			"GraphicsBase",

			"CivetWeb",

			-- Vulkan
		}

		dependson(deps)
		links(deps)

		filter { "system:windows" }
			links { "d3d11", "dxgi" }
			--table.insert(deps, "D3dcompiler")
			--table.insert(deps, "dxguid")

		filter{}
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevGraphics")
	table.insert(deps, "Gleam")
	table.insert(deps, "DevWebServer")
	--table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "CivetWeb")

	dependson(deps)
	links(deps)

	filter { "system:windows" }
		links { "d3d11", "dxgi" }
		--table.insert(deps, "D3dcompiler")
		--table.insert(deps, "dxguid")

	filter{}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
