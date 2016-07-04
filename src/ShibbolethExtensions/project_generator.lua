require "../../utils/premake5_extensions/premake-qt/qt"
local qt = premake.extensions.qt

group "Editor"

project "ShibbolethExtensions"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/shibbolethextensions")
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

	kind "SharedLib"
	language "C++"
	warnings "Default"

	includedirs { "../../dependencies/Contrivance/ContrivanceLib" }
	files { "*.h", "*.cpp", "*.ui" }

	dependson { "ContrivanceLib" }
	links { "ContrivanceLib" }

	qtmodules { "core", "widgets", "gui" }

	vpaths
	{
		["Generated"] = { "../../build/intermediate/*.*" },
		[""] = { "*.*"}
	}
