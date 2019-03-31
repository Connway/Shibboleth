local GenerateProject = function()
	local base_dir = GetModulesDirectory("MainLoop")

	project "MainLoop"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl", base_dir .. "**.lua" }
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
			base_dir .. "../../Dependencies/EASTL/include"
		}

	project "MainLoopModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_MainLoopModule.cpp" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("MainLoop")
		SetupConfigMap()
		ModuleCopy()
end

local LinkDependencies = function()
	local deps = ModuleDependencies("MainLoop")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
