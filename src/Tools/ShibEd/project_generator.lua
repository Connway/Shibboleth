if _OPTIONS["no-editor"] then
	return
end

require("premake-qt/qt")
local qt = premake.extensions.qt

project "ShibEd"
	-- If defaultpath is nil, then we are generating a header. Do nothing.
	if qt.defaultpath == nil then
		return
	end

	qt.enable()

	if _ACTION then
		location(GetToolsLocation())
	end

	kind "ConsoleApp"
	language "C++"
	warnings "Extra"
	debugdir "../../../workingdir/tools"

	files { "**.h", "**.cpp", "**.qrc", "**.ui" }
	excludes { "**/moc*.*" }
	defines { "QT_DISABLE_DEPRECATED_BEFORE=0x060000", "SHIB_STATIC" }

	includedirs
	{
		".",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Modules/Graphics/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/ads"
	}

	local deps =
	{
		"Engine",
		"Gaff",
		"Gleam",
		"Memory",
		"EASTL",
		"mpack",

		"ads"
	}

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "Dbghelp", "iphlpapi", "psapi", "userenv" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPYDIR} ../../../../../workingdir/bin ../../../../../workingdir/tools",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}

	qtgenerateddir ".generated"
	qtprefix "Qt6"

	local modules =
	{
		"core",
		"gui",
		"widgets"
	}

	qtmodules(modules)

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
		"{MKDIR} ../../../../../workingdir/tools/platforms",
		"{COPY} " .. plugin_path .. "/platforms/qwindowsd" .. extension .. " ../../../../../workingdir/tools/platforms",
		"{COPY} " .. plugin_path .. "/platforms/qwindows" .. extension .. " ../../../../../workingdir/tools/platforms"
	}

	for _, name in ipairs(modules) do
		local final_name = bin_path .. "/Qt6" .. qt.modules[name].name

		postbuildcommands
		{
			"{COPY} " .. final_name .. "d" .. extension .. " ../../../../../workingdir/tools",
			"{COPY} " .. final_name .. extension .." ../../../../../workingdir/tools"
		}
	end

	SetupConfigMap()
	ToolGen("ShibEd", ".generated")
