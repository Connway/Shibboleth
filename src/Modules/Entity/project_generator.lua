local GenerateProject = function()
	local base_dir = GetModulesDirectory("Entity")

	project "Entity"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Entity")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include"
		}


	project "EntityModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_EntityModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Entity")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"MainLoop"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Entity")
	table.insert(deps, "MainLoop")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
