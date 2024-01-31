local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Input")
	local base_dir = GetModulesDirectory("Input")

	ModuleProject "Input"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/Player/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include"
		}

	ModuleProject "InputModule"
		language "C++"

		files { source_dir .. "Shibboleth_InputModule.cpp" }

		local deps =
		{
			"Gleam",
			"MainLoop",
			"Player",
			"Resource",

			--"GLFW"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Input")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Player")
	table.insert(deps, "Resource")
	--table.insert(deps, "GLFW")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
