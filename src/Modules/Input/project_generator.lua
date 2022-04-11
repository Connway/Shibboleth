local GenerateProject = function()
	local base_dir = GetModulesDirectory("Input")

	project "Input"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Input")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Modules/Graphics/include" -- for creating keyboard/mouse
		}

	project "InputModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_InputModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Input")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"Gleam",
			"MainLoop",
			"GraphicsBase"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Input")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "GraphicsBase")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
