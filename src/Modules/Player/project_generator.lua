local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Player")
	local base_dir = GetModulesDirectory("Player")

	ModuleProject "Player"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Memory/include",
			source_dir .. "../../Core/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include"
		}

	ModuleProject "PlayerModule"
		language "C++"

		files { source_dir .. "Shibboleth_PlayerModule.cpp" }

		local deps =
		{
			"Player"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Player")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
