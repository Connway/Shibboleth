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
			base_dir,
			base_dir .. "include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/glm"
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
