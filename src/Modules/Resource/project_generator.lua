local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Resource")
	local base_dir = GetModulesDirectory("Resource")

	ModuleProject "Resource"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/DevDebug/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/imgui",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Frameworks/Gaff/include"
		}


	ModuleProject "ResourceModule"
		language "C++"

		files { source_dir .. "src/Shibboleth_ResourceModule.cpp" }

		local deps =
		{
			"imgui",

			"MainLoop",
			"DevDebug"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Resource")
	table.insert(deps, "imgui")
	table.insert(deps, "MainLoop")
	table.insert(deps, "DevDebug")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
