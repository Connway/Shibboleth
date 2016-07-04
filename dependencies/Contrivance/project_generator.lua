require "../../utils/premake5_extensions/premake-qt/qt"
local qt = premake.extensions.qt

group "Editor"

project "ContrivanceLib"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	if _OPTIONS["qt_dir"] then
		premake.extensions.qt.defaultpath = _OPTIONS["qt_dir"]
	else
		if os.getenv("QTDIR") then
			premake.extensions.qt.defaultpath = "$(QTDIR)"
		elseif os.getenv("QT_DIR") then
			premake.extensions.qt.defaultpath = "$(QT_DIR)"
		end
	end

	if _OPTIONS["qt_prefix"] then
		qtprefix(_OPTIONS["qt_prefix"])
	end

	qt.enable()

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"
	warnings "Default"

	files
	{
		"ContrivanceLib/**.h",
		"ContrivanceLib/**.cpp",
		"ContrivanceLib/**.ui"
	}

	qtmodules { "core", "widgets", "gui" }

	vpaths
	{
		["Generated"] = { "../../build/intermediate/*.*" },
		[""] = { "ContrivanceLib/*.*"}
	}

project "Contrivance"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	if _OPTIONS["qt_dir"] then
		premake.extensions.qt.defaultpath = _OPTIONS["qt_dir"]
	else
		if os.getenv("QTDIR") then
			premake.extensions.qt.defaultpath = "$(QTDIR)"
		elseif os.getenv("QT_DIR") then
			premake.extensions.qt.defaultpath = "$(QT_DIR)"
		end
	end

	if _OPTIONS["qt_prefix"] then
		qtprefix(_OPTIONS["qt_prefix"])
	else
		qtprefix "Qt5"
	end

	configuration { "Debug" }
		qtsuffix "d"

	configuration {}

	qt.enable()

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	dofile("../../utils/module_suffix.lua")

	kind "WindowedApp"
	debugdir "../../workingdir/Editor"
	language "C++"
	warnings "Default"

	includedirs { "ContrivanceLib" }
	files { "*.h", "*.cpp", "*.ui" }

	dependson { "ContrivanceLib" }
	links { "ContrivanceLib" }

	qtmodules { "core", "widgets", "gui" }

group "Dependencies"
