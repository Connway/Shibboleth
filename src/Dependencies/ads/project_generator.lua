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
