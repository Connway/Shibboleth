local GenerateProject = function()
	local base_dir = GetModulesDirectory("Camera")

	project "Camera"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Camera")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Frameworks/Esprit/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/Input/include",
			base_dir .. "../../Modules/ECS/include"
		}

	project "CameraModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_CameraModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Camera")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"Esprit",
			"Gleam",
			"MainLoop",
			"Resource",
			"Input",
			"ECS"
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

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
