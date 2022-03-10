if _OPTIONS["no-editor"] then
	return
end

require("premake-qt/qt")
local qt = premake.extensions.qt

project "ads"
	-- If defaultpath is nil, then we are generating a header. Do nothing.
	if qt.defaultpath == nil then
		return
	end

	qt.enable()

	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "SharedLib"
	language "C++"
	warnings "Extra"

	files { "**.h", "**.cpp", "**.qrc", "**.ui" }
	defines { "QT_DISABLE_DEPRECATED_BEFORE=0x060000", "ADS_SHARED_EXPORT" }

	qtgenerateddir ".generated"
	qtmodules { "core", "gui", "widgets" }
	qtprefix "Qt6"

	filter { "system:windows" }
		removefiles { "linux/*.*" }

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

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}

	SetupConfigMap()
