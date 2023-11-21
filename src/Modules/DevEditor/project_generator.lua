if _OPTIONS["no-editor"] then
	return { GenerateProject = function() end, LinkDependencies = function() end }
end

require("premake-qt/qt")
local qt = premake.extensions.qt

local qt_modules =
{
	"core",
	"gui",
	"widgets"
}

local GenerateProject = function()
	Group "Modules/Dev"

	local source_dir = GetModulesSourceDirectory("DevEditor")
	local base_dir = GetModulesDirectory("DevEditor")

	DevModuleProject "DevEditor"
		qt.enable()

		language "C++"

		IncludeDirs
		{
			source_dir,
			source_dir .. "include",
			source_dir .. "../../Core/Engine/include",
			source_dir .. "../../Core/Memory/include",
			base_dir .. "../../Frameworks/Gaff/include",
			--source_dir .. "../../Modules/Graphics/include",
			source_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/ads"
		}

		QtSettings(qt_modules, base_dir, source_dir)

	DevModuleProject "DevEditorModule"
		qt.enable()

		language "C++"

		files { source_dir .. "Shibboleth_DevEditorModule.cpp" }

		local deps =
		{
			"Gleam",
			"MainLoop",

			"ads"
		}

		dependson(deps)
		links(deps)

		filter { "system:windows" }
			links { "Dbghelp", "iphlpapi", "psapi", "userenv" }

		filter {}

		QtSettingsModule(qt_modules, base_dir, source_dir)
		CopyQtFilesToBin(qt_modules)

	Group "Modules"
end

local LinkDependencies = function()
	local deps = ModuleDependencies("DevEditor")
	table.insert(deps, "Gleam")
	table.insert(deps, "MainLoop")
	table.insert(deps, "ads")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
