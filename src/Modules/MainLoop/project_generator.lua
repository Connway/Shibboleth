local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("MainLoop")
	local base_dir = GetModulesDirectory("MainLoop")

	ModuleProject "MainLoop"
		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

		IncludeDirs
		{
			source_dir .. "include",
			source_dir .. "../../Engine/Engine/include",
			source_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/glm"
		}

	ModuleProject "MainLoopModule"
		language "C++"

		files { source_dir .. "Shibboleth_MainLoopModule.cpp" }

		--[[local deps =
		{
		}

		dependson(deps)
		links(deps)--]]
end

local LinkDependencies = function()
	local deps = ModuleDependencies("MainLoop")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
