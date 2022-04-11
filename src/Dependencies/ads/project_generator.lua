if _OPTIONS["no-editor"] then
	return
end

require("premake-qt/qt")
local qt = premake.extensions.qt

project "ads"
	qt.enable()

	location(GetDependenciesLocation())

	kind "SharedLib"
	language "C++"
	warnings "Extra"

	QtSettings({ "core", "gui", "widgets" })

	defines { "ADS_SHARED_EXPORT" }

	filter { "system:windows" }
		removefiles { "linux/*.*" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}

	SetupConfigMap()
