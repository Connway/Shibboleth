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

	if os.target() == "linux" or os.target() == "macosx" then
		local version_number = qt.defaultpath

		if version_number:sub(1, 1) == "$" then
			version_number = os.getenv(version_number:sub(3, version_number:len() - 1))
		end

		if os.host() == "windows" then
			version_number = path.translate(version_number, "/")
		end

		local last_separator = version_number:findlast("/", true)
		version_number = version_number:sub(1, last_separator - 1)

		last_separator = version_number:findlast("/", true)
		version_number = version_number:sub(last_separator + 1)

		includedirs { qt.defaultpath .. "/include/QtGui/" .. version_number .. "/QtGui" }
	end

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
