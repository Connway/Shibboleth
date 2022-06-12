local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Resource")
	local base_dir = GetModulesDirectory("Resource")

	ModuleProject "Resource"
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
			source_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include"
		}


	ModuleProject("ResourceModule", "SharedLib")
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "Shibboleth_ResourceModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Resource")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy()

		local deps =
		{
			"MainLoop"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Resource")
	table.insert(deps, "MainLoop")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
