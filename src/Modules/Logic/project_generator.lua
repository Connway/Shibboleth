local GenerateProject = function()
	local base_dir = GetModulesDirectory("Logic")

	project "Logic"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Logic")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/lua",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/tracy",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Esprit/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/Script/include",
			base_dir .. "../../Modules/ECS/include"
		}

	project "LogicModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_LogicModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Logic")
		NewDeleteLinkFix()
		SetupConfigMap()

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
