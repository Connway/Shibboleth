local GenerateProject = function()
	local base_dir = GetModulesDirectory("EditorBasic")

	project "EditorBasic"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("EditorBasic")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Engine/Editor/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../Graphics/include"
		}

		IncludeWxWidgets()
		SetupConfigMap()



	project "EditorBasicModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_EditorBasicModule.cpp" }

		ModuleEditorCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("EditorBasic")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"Editor",
			"Gleam",
			"libpng",
			"zlib-ng",
			"wxBase",
			"wxCore",
			"wxPropGrid"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("EditorBasic")
	table.insert(deps, "Editor")
	table.insert(deps, "Gleam")
	table.insert(deps, "libpng")
	table.insert(deps, "zlib-ng")
	table.insert(deps, "wxBase")
	table.insert(deps, "wxCore")
	table.insert(deps, "wxPropGrid")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
