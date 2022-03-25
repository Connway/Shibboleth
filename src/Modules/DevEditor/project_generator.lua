if _OPTIONS["no-editor"] then
	return
end

require("premake-qt/qt")
local qt = premake.extensions.qt

local QtModules =
{
	"core",
	"gui",
	"widgets"
}

local QtSettings = function(base_dir)
	defines { "QT_DISABLE_DEPRECATED_BEFORE=0x060000" }

	qtgenerateddir(base_dir .. ".generated")
	qtprefix "Qt6"

	qtmodules(QtModules)

	filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Debug*", "platforms:x64" }
		qtsuffix "d"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Release*", "platforms:x64" }
	-- 	qtsuffix "64"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Profile*", "platforms:x64" }
	-- 	qtsuffix "64p"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Optimized_Debug*", "platforms:x64" }
	-- 	qtsuffix "64od"

	filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Static_Debug*", "platforms:x64" }
		qtsuffix "d"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Static_Release*", "platforms:x64" }
	-- 	qtsuffix "64s"

	filter {}
end

local GenerateProject = function()
	-- If defaultpath is nil, then we are generating a header. Do nothing.
	if qt.defaultpath == nil then
		return
	end

	local base_dir = GetModulesDirectory("DevEditor")

	project "DevEditor"
		qt.enable()

		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.qrc", base_dir .. "**.ui" }
		excludes { base_dir .. "**/moc*.*" }
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

		QtSettings(base_dir)

	project "DevEditorModule"
		qt.enable()

		if _ACTION then
			location(GetModulesLocation())
		end

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

		QtSettings(base_dir)

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

		for _, name in ipairs(QtModules) do
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
