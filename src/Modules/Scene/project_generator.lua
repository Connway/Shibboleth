local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Scene")
	local base_dir = GetModulesDirectory("Scene")

	ModuleProject "Scene"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
			-- source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Frameworks/Gaff/include",
		}


	ModuleProject "SceneModule"
		language "C++"

		files { source_dir .. "Shibboleth_SceneModule.cpp" }

		local deps =
		{
			--"MainLoop",
			"Resource",
			"DevDebug",
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Scene")
	--table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "DevDebug")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
