local GenerateProject = function()
	local base_dir = GetModulesDirectory("Debug")

	project "Debug"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
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
			base_dir .. "../../Dependencies/glfw/include",
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
		location(GetModulesLocation())

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
			"Assimp",
			"imgui",
			"minizip-ng",
			"zlib-ng",
			"GLFW",
			"libpng",
			"libtiff",

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
	table.insert(deps, "Assimp")
	table.insert(deps, "imgui")
	table.insert(deps, "minizip-ng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "GLFW")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")

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
