local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("ECS")
	local base_dir = GetModulesDirectory("ECS")

	ModuleProject "ECS"
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
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/DevWebServer/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/Resource/include"
		}

	ModuleProject("ECSModule", "SharedLib")
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "Shibboleth_ECSModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("ECS")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy()

		local deps =
		{
			"Gleam",

			"MainLoop",
			"Resource"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("ECS")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
