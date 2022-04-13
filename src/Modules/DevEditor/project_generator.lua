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

print("Qt Default Path: " .. qt.defaultpath)

local GenerateProject = function()
	local base_dir = GetModulesDirectory("DevEditor")

	project "DevEditor"
		qt.enable()

		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		defines { "SHIB_STATIC" }

		SetupConfigMap()
		ModuleGen("DevEditor")

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Frameworks/Gaff/include",
			--base_dir .. "../../Modules/Graphics/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/ads"
		}

		QtSettings(qt_modules, base_dir)

	project "DevEditorModule"
		qt.enable()

		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_DevEditorModule.cpp" }

		ModuleCopy("DevModules")

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("DevEditor")
		NewDeleteLinkFix()
		SetupConfigMap()

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

		QtSettingsModule(qt_modules, base_dir)

		local plugin_path = qt.defaultpath .. "/plugins"
		local bin_path = qt.defaultpath .. "/bin"
		local extension = ".dll" -- Windows extension by default.

		if os.target() == "linux" then
			extension = ".so"
		elseif os.target() == "macosx" then
			extension = ".dylib"
		end

		postbuildcommands
		{
			"{MKDIR} ../../../../../workingdir/bin",
			"{MKDIR} ../../../../../workingdir/bin/platforms",
			"{COPYFILE} " .. plugin_path .. "/platforms/qwindowsd" .. extension .. " ../../../../../workingdir/bin/platforms",
			"{COPYFILE} " .. plugin_path .. "/platforms/qwindows" .. extension .. " ../../../../../workingdir/bin/platforms"
		}

		for _, name in ipairs(qt_modules) do
			local final_name = bin_path .. "/Qt6" .. qt.modules[name].name

			postbuildcommands
			{
				"{COPYFILE} " .. final_name .. "d" .. extension .. " ../../../../../workingdir/bin",
				"{COPYFILE} " .. final_name .. extension .." ../../../../../workingdir/bin"
			}
		end
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
