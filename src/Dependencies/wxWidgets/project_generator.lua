function wxWidgetsProject(proj_name)
	project(proj_name)

	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C++"

	defines
	{
		"NOPCH",
		"WXBUILDING",
		"wxUSE_ZLIB_H_IN_PATH",
		"__WXMSW__",
		"_CRT_SECURE_NO_DEPRECATE=1",
		"_CRT_NON_CONFORMING_SWPRINTFS=1",
		"_SCL_SECURE_NO_WARNINGS=1"
	}

	includedirs { "include" }

	-- pchheader "wx/wxprec.h"
	-- pchsource "src/common/dummpy.cpp"

	filter { "system:windows" }
		includedirs { "include/msvc" }
		defines { "__WXMSW__" }

	filter {}

	dofile(string.lower(proj_name) .. "_files.lua")
end

group "Dependencies/wxWidgets"

wxWidgetsProject "wxBase"
	includedirs { "src/regex", "../zlib-ng" }
	defines { "wxUSE_GUI=0", "wxUSE_BASE=1" }

wxWidgetsProject "wxCore"
	includedirs { "../libjpeg", "../libpng", "../libtiff", "../zlib-ng" }
	defines { "wxUSE_BASE=0" }

wxWidgetsProject "wxXML"
	includedirs { "../expat" }

wxWidgetsProject "wxAdv"
wxWidgetsProject "wxAui"
wxWidgetsProject "wxHTML"
wxWidgetsProject "wxMedia"
-- wxWidgetsProject "wxNet"
wxWidgetsProject "wxPropGrid"
wxWidgetsProject "wxQA"
wxWidgetsProject "wxRegex"
wxWidgetsProject "wxRibbon"
wxWidgetsProject "wxRichText"
wxWidgetsProject "wxWebview"
wxWidgetsProject "wxXRC"

group "Dependencies"
