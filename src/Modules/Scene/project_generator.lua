local GenerateProject = function()
	local base_dir = GetModulesDirectory("Scene")

	project "Scene"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Scene")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			-- base_dir .. "../../Dependencies/rapidjson",
			-- base_dir .. "../../Dependencies/glm",
			-- base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Frameworks/Gaff/include",
			-- base_dir .. "../../Frameworks/Gleam/include"
		}

	project "SceneModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_SceneModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Scene")
		SetupConfigMap()
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Scene")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
