local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Camera")
	local base_dir = GetModulesDirectory("Camera")

	ModuleProject "Camera"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()

		flags { "FatalWarnings" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Frameworks/Esprit/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Input/include",
			source_dir .. "../../Modules/ECS/include"
		}

	ModuleProject("CameraModule", "SharedLib")
		language "C++"

		files { source_dir .. "Shibboleth_CameraModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Camera")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy()

		local deps =
		{
			"Esprit",
			"Gleam",
			"MainLoop",
			"Resource",
			"Input",
			"ECS",
			"GLFW"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Camera")
	table.insert(deps, "Esprit")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Input")
	table.insert(deps, "ECS")
	table.insert(deps, "GLFW")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
