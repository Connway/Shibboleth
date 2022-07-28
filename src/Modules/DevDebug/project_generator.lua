local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("DevDebug")
	local base_dir = GetModulesDirectory("DevDebug")

	ModuleProject "DevDebug"
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
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/imgui",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Dependencies/CivetWeb/include",
			source_dir .. "../../Modules/DevWebServer/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/Graphics/include",
			source_dir .. "../../Modules/Input/include",
			source_dir .. "../../Modules/ECS/include"
		}

	ModuleProject("DevDebugModule", "SharedLib")
		language "C++"

		files { source_dir .. "Shibboleth_DevDebugModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("DevDebug")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy("DevModules")

		local deps =
		{	
			"Assimp",
			"imgui",
			"minizip-ng",
			"zlib-ng",
			"libpng",
			"libtiff",
			"CivetWeb",
			"GLFW",

			"Gleam",

			"DevWebServer",
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
	local deps = ModuleDependencies("DevDebug")
	table.insert(deps, "Assimp")
	table.insert(deps, "imgui")
	table.insert(deps, "minizip-ng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")
	table.insert(deps, "CivetWeb")
	table.insert(deps, "GLFW")

	table.insert(deps, "Gleam")

	table.insert(deps, "DevWebServer")
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Input")
	table.insert(deps, "ECS")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
