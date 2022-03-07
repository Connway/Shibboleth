local GenerateProject = function()
	local base_dir = GetModulesDirectory("DevWebServer")

	project "DevWebServer"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl", base_dir .. "**.lua" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()
		ModuleGen("DevWebServer")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/CivetWeb/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			-- base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../Input/include"
		}

	project "DevWebServerModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_DevWebServerModule.cpp" }

		ModuleCopy("DevModules")

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("DevWebServer")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"CivetWeb",

			"Gleam",

			"MainLoop",
			"Input"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevWebServer")
	table.insert(deps, "CivetWeb")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Input")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
