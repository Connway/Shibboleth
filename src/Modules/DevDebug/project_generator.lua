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
			base_dir .. "../../Dependencies/CivetWeb/include",
			base_dir .. "../../Modules/DevWebServer/include",
			base_dir .. "../../Modules/Debug/include",
			--base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include"
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
			"Gleam",

			"DevWebServer",
			--"Debug",

			"CivetWeb",
			"GLFW"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevDebug")
	table.insert(deps, "Gleam")
	table.insert(deps, "DevWebServer")
	--table.insert(deps, "MainLoop")
	--table.insert(deps, "Resource")
	--table.insert(deps, "Debug")
	table.insert(deps, "CivetWeb")
	table.insert(deps, "GLFW")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
