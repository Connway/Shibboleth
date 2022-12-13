local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("DevGraphics")
	local base_dir = GetModulesDirectory("DevGraphics")

	DevModuleProject "DevGraphics"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/CivetWeb/include",
			base_dir .. "../../Dependencies/nvenc",
			source_dir .. "../../Modules/DevWebServer/include",
			source_dir .. "../../Modules/ECS/include",
			source_dir .. "../../Modules/Graphics/include",
			source_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include"
		}

	DevModuleProject "DevGraphicsModule"
		language "C++"

		files { source_dir .. "Shibboleth_DevGraphicsModule.cpp" }

		local deps =
		{
			"Gleam",

			"DevWebServer",
			"DevDebug",
			--"MainLoop",
			"Resource",
			"GraphicsBase",

			"CivetWeb",
			"GLFW",
			"libpng",
			"libtiff",
			"zlib-ng"

			-- Vulkan
		}

		dependson(deps)
		links(deps)

		filter { "system:windows" }
			links { "d3d11", "dxgi" }
			--table.insert(deps, "D3dcompiler")
			--table.insert(deps, "dxguid")

		filter {}
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevGraphics")
	table.insert(deps, "Gleam")
	table.insert(deps, "DevWebServer")
	table.insert(deps, "DevDebug")
	--table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "CivetWeb")
	table.insert(deps, "GLFW")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")
	table.insert(deps, "zlib-ng")

	dependson(deps)
	links(deps)

	filter { "system:windows" }
		links { "d3d11", "dxgi" }
		--table.insert(deps, "D3dcompiler")
		--table.insert(deps, "dxguid")

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
