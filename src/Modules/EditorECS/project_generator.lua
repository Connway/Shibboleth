local GenerateProject = function()
	local base_dir = GetModulesDirectory("EditorECS")

	project "EditorECS"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("EditorECS")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Engine/Editor/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../ECS/include"
		}

		IncludeWxWidgets()
		SetupConfigMap()



	project "EditorECSModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_EditorECSModule.cpp" }

		ModuleEditorCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("EditorECS")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"Editor",
			"libpng",
			"zlib-ng",
			"wxBase",
			"wxCore",
			"ECS"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("EditorECS")
	table.insert(deps, "Editor")
	table.insert(deps, "libpng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "wxBase")
	table.insert(deps, "wxCore")
	table.insert(deps, "ECS")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
