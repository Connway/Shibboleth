local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("DevECS")
	local base_dir = GetModulesDirectory("DevECS")

	ModuleProject "DevECS"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			source_dir .. "include",
			source_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/CivetWeb/include",
			source_dir .. "../../Modules/DevWebServer/include",
			source_dir .. "../../Modules/ECS/include",
			source_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include"
		}

	ModuleProject("DevECSModule", "SharedLib")
		language "C++"

		files { source_dir .. "Shibboleth_DevECSModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("DevECS")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy("DevModules")

		local deps =
		{
			"Gleam",

			"DevWebServer",
			"MainLoop",
			"Resource",
			"ECS",

			"CivetWeb"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevECS")
	table.insert(deps, "Gleam")
	table.insert(deps, "DevWebServer")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "ECS")
	table.insert(deps, "CivetWeb")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
