local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Entity")
	local base_dir = GetModulesDirectory("Entity")

	ModuleProject "Entity"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
			source_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include"
		}

	ModuleProject "EntityModule"
		language "C++"

		files { source_dir .. "Shibboleth_EntityModule.cpp" }

		local deps =
		{
			"MainLoop"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Entity")
	table.insert(deps, "MainLoop")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
