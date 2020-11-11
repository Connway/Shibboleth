local GenerateProject = function()
	local base_dir = GetModulesDirectory("Debug")

	project "Debug"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl", base_dir .. "**.lua" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()
		ModuleGen("Debug")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/imgui",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../MainLoop/include",
			base_dir .. "../Resource/include",
			base_dir .. "../Graphics/include",
			base_dir .. "../Input/include",
			base_dir .. "../ECS/include"
		}

	project "DebugModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_DebugModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Debug")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"imgui",
			"Gleam",
			"GraphicsBase",
			"MainLoop",
			"Resource",
			"Input",
			"ECS"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Debug")
	table.insert(deps, "imgui")
	table.insert(deps, "Gleam")
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Input")
	table.insert(deps, "ECS")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
