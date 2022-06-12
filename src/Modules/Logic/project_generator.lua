local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Logic")
	local base_dir = GetModulesDirectory("Logic")

	ModuleProject "Logic"
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			source_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
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
			source_dir .. "../../Modules/ECS/include"
		}

	ModuleProject("LogicModule", "SharedLib")
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "Shibboleth_LogicModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Logic")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy()

		local deps =
		{
			"Esprit",

			"MainLoop",
			"Resource",
			"Script",
			"ECS",

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
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Script")
	table.insert(deps, "ECS")
	table.insert(deps, "Lua")

	dependson(deps)
	links(deps)

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
