local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("DevWebServer")
	local base_dir = GetModulesDirectory("DevWebServer")

	DevModuleProject "DevWebServer"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/CivetWeb/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			-- base_dir .. "../../Dependencies/mpack",
			source_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/Input/include"
		}

	DevModuleProject "DevWebServerModule"
		language "C++"

		files { source_dir .. "Shibboleth_DevWebServerModule.cpp" }

		local deps =
		{
			"CivetWeb",
			"GLFW",

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
	table.insert(deps, "GLFW")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Input")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
