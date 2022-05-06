local GenerateProject = function()
	local base_dir = GetModulesDirectory("DevDebug")

	project "DevDebug"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()
		ModuleGen("DevDebug")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glfw/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/imgui",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Dependencies/CivetWeb/include",
			base_dir .. "../../Modules/DevWebServer/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/Graphics/include",
			base_dir .. "../../Modules/Input/include",
			base_dir .. "../../Modules/ECS/include"
		}

	project "DevDebugModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_DevDebugModule.cpp" }

		ModuleCopy("DevModules")

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("DevDebug")
		NewDeleteLinkFix()
		SetupConfigMap()

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
