local GenerateProject = function()
	local base_dir = GetModulesDirectory("EditorMainLoop")

	project "EditorMainLoop"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("EditorMainLoop")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../Graphics/include"
		}

	project "EditorMainLoopModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_EditorMainLoopModule.cpp" }

		ModuleEditorCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("EditorMainLoop")
		SetupConfigMap()

		local deps =
		{
			"Gleam"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("EditorMainLoop")
	table.insert(deps, "Gleam")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
