local module_link_deps =
{
	"MainLoop",
	"Resource",
	"Entity",
	"DevDebug",
	"Gleam"
}

local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Scene")
	local base_dir = GetModulesDirectory("Scene")

	ModuleProject "Scene"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			-- source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Entity/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Frameworks/Gaff/include",
		}


	ModuleProject "SceneModule"
		language "C++"

		files { source_dir .. "src/Shibboleth_SceneModule.cpp" }

		dependson(module_link_deps)
		links(module_link_deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Scene")
	deps = table.join(deps, module_link_deps)

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
