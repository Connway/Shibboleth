local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Camera")
	local base_dir = GetModulesDirectory("Camera")

	ModuleProject "Camera"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Frameworks/Esprit/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Input/include"
		}

	ModuleProject "CameraModule"
		language "C++"

		files { source_dir .. "src/Shibboleth_CameraModule.cpp" }

		local deps =
		{
			"Esprit",
			"Gleam",
			"MainLoop",
			"DevDebug",
			"Resource",
			"Input",
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Camera")
	table.insert(deps, "Esprit")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "DevDebug")
	table.insert(deps, "Resource")
	table.insert(deps, "Input")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
