local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Entity")
	local base_dir = GetModulesDirectory("Entity")

	ModuleProject "Entity"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
		}

	ModuleProject "EntityModule"
		language "C++"

		files { source_dir .. "src/Shibboleth_EntityModule.cpp" }

		local deps =
		{
			"MainLoop",
			"Resource",
			"DevDebug",
			"Gleam"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Entity")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "DevDebug")
	table.insert(deps, "Gleam")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
