local GenerateProject = function()
	local base_dir = GetModulesDirectory("Script")

	project "Script"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Script")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/lua",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/tracy",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/ECS/include"
		}

	project "ScriptModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_ScriptModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Script")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"MainLoop",
			"Resource",
			"ECS",

			"Lua",

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
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "ECS")

	table.insert(deps, "Lua")

	table.insert(deps, "Gleam")

	dependson(deps)
	links(deps)

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
