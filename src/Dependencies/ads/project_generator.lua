if _OPTIONS["no-editor"] then
	return
end

require("premake-qt/qt")
local qt = premake.extensions.qt

DependencyProject("ads", "SharedLib")
	qt.enable()

	language "C++"
	warnings "Extra"

	QtSettings({ "core", "gui", "widgets" })

	defines { "ADS_SHARED_EXPORT" }
	includedirs { "." }

	filter { "configurations:*Debug*"}
		defines { "ADS_DEBUG_PRINT" }

	filter { "system:not linux" }
		removefiles { "linux/*.*" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	SetupConfigMap()
