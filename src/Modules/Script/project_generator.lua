local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Script")
	local base_dir = GetModulesDirectory("Script")

	ModuleProject "Script"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/" .. GetLuaIncDep(),
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/tracy",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/ECS/include"
		}

	ModuleProject "ScriptModule"
		language "C++"

		files { source_dir .. "Shibboleth_ScriptModule.cpp" }

		local deps =
		{
			"DevDebug",
			"MainLoop",
			"Resource",
			"ECS",

			GetLuaLinkDep(),
			GetLuaDepDep(),

			"Gleam",
		}

		dependson(deps)
		links(deps)

		filter { "configurations:*Debug* or *Profile*" }
			dependson({ "TracyClient" })
			links({ "TracyClient" })

		filter {}
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Script")
	table.insert(deps, "DevDebug")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "ECS")

	table.insert(deps, GetLuaLinkDep())
	table.insert(deps, GetLuaDepDep())

	table.insert(deps, "Gleam")

	dependson(deps)
	links(deps)

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
