local GenerateProject = function()
	Group "Modules/Dev"

	local source_dir = GetModulesSourceDirectory("DevDebug")
	local base_dir = GetModulesDirectory("DevDebug")

	DevModuleProject "DevDebug"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/imgui",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Graphics/include",
			source_dir .. "../../Modules/Input/include"
		}

		GleamRendererDefines()

	DevModuleProject "DevDebugModule"
		language "C++"

		files { source_dir .. "Shibboleth_DevDebugModule.cpp" }

		GleamRendererDefines()
		GleamRendererLinks()

		local deps =
		{
			"Assimp",
			"imgui",
			"minizip-ng",
			"zlib-ng",
			"zstd",
			"libpng",
			"libtiff",
			"GLFW",

			"Gleam",

			"Graphics",
			"MainLoop",
			"Resource",
			"Input"
		}

		dependson(deps)
		links(deps)

	Group "Modules"
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevDebug")
	table.insert(deps, "Assimp")
	table.insert(deps, "imgui")
	table.insert(deps, "minizip-ng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "zstd")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")
	table.insert(deps, "GLFW")

	table.insert(deps, "Gleam")

	table.insert(deps, "Graphics")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Input")

	for _,v in pairs(GleamRendererLinksArray()) do
		table.insert(v)
	end

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
