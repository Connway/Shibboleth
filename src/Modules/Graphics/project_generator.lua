local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Graphics")
	local base_dir = GetModulesDirectory("Graphics")

	ModuleProject "Graphics"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
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

		files { source_dir .. "src/Shibboleth_GraphicsModule.cpp" }

		GleamRendererDefines()
		GleamRendererLinks()

		local deps =
		{
			"MainLoop",
			"Resource",
			"Gleam",
			"Entity",
			"assimp",
			"mpack",
			"minizip-ng",
			"zlib-ng",
			"zstd",
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
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "DevDebug")
	table.insert(deps, "Gleam")
	table.insert(deps, "Entity")
	table.insert(deps, "assimp")
	table.insert(deps, "mpack")
	table.insert(deps, "minizip-ng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "zstd")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")
	table.insert(deps, "GLFW")

	for _,v in pairs(GleamRendererLinksArray()) do
		table.insert(v)
	end

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
