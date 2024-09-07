local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Logic")
	local base_dir = GetModulesDirectory("Logic")

	ModuleProject "Logic"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/lua",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/tracy",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Esprit/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Script/include",
			source_dir .. "../../Modules/Entity/include"
		}

	ModuleProject "LogicModule"
		language "C++"

		files { source_dir .. "src/Shibboleth_LogicModule.cpp" }

		local deps =
		{
			"Esprit",
			"Gleam",

			"DevDebug",
			"MainLoop",
			"Resource",
			"Script",
			"Entity",

			"Lua"
		}

		dependson(deps)
		links(deps)

		filter { "configurations:*Debug* or *Profile*" }
			dependson({ "TracyClient" })
			links({ "TracyClient" })

		filter {}
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Logic")
	table.insert(deps, "Esprit")
	table.insert(deps, "Gleam")
	table.insert(deps, "DevDebug")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Script")
	table.insert(deps, "Entity")
	table.insert(deps, "Lua")

	dependson(deps)
	links(deps)

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
