local GenerateProject = function()
	local base_dir = GetModulesDirectory("MainLoop")

	project "MainLoop"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("MainLoop")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm"

			, base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/ECS/include",
			base_dir .. "../../Modules/Graphics/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson"
		}

	project "MainLoopModule"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_MainLoopModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("MainLoop")
		SetupConfigMap()
		ModuleCopy()
		NewDeleteLinkFix()

		local deps =
		{
			"Resource",
			"ECS",
			"GraphicsBase",
			"Gleam",
			"Assimp",
			"zlib-ng",
			"minizip",
			"libpng",
			"libtiff"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("MainLoop")

	-- temp
	table.insert(deps, "Resource")
	table.insert(deps, "ECS")
	table.insert(deps, "GraphicsBase")
	table.insert(deps, "Gleam")
	table.insert(deps, "Assimp")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "minizip")
	table.insert(deps, "libpng")
	table.insert(deps, "libtiff")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
