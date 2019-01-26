function wxWidgetsProject(proj_name, lib_type)
	project(proj_name)

	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind(lib_type)
	language "C++"

	defines
	{
		"NOPCH",
		"WXBUILDING",
		"wxUSE_ZLIB_H_IN_PATH",
		"__WXMSW__",
		"_CRT_SECURE_NO_DEPRECATE=1",
		"_CRT_NON_CONFORMING_SWPRINTFS=1",
		"_SCL_SECURE_NO_WARNINGS=1",
		"XML_STATIC"
	}

	-- To use our overrided new/delete.
	links { "Engine", "Memory" }

	if lib_type == "SharedLib" then
		defines { "WXMAKINGDLL_" .. proj_name:sub(3):upper() }
	end

	if proj_name ~= "wxBase" then
		defines { "WXUSINGDLL" }
	end

	includedirs { "include" }

	-- pchheader "wx/wxprec.h"
	-- pchsource "src/common/dummpy.cpp"

	filter { "system:windows" }
		includedirs { "include/msvc" }
		defines { "__WXMSW__" }

	filter {}

	if lib_type == "SharedLib" then
		postbuildcommands
		{
			"{MKDIR} ../../../../../workingdir/bin",
			"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
		}
	end

	dofile(string.lower(proj_name) .. "_files.lua")
end

group "Dependencies/wxWidgets"

wxWidgetsProject("wxBase", "SharedLib")
	includedirs
	{
		"src/regex",
		"../zlib-ng",
		"../../Frameworks/Gaff/include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include"
	}

	defines { "wxUSE_GUI=0", "wxUSE_BASE=1" }

	local base_deps = { "zlib-ng", "wxRegex", "Memory" }
	dependson(base_deps)
	links(base_deps)


wxWidgetsProject("wxCore", "SharedLib")
	includedirs { "../libjpeg", "../libpng", "../libtiff", "../zlib-ng" }
	defines { "wxUSE_BASE=0" }

	local core_deps = { "wxBase", "libjpeg", "libpng", "libtiff", "zlib-ng" }
	dependson(core_deps)
	links(core_deps)

	filter { "system:windows" }
		links { "Comctl32", "Rpcrt4", "Winmm" }

	filter {}


wxWidgetsProject("wxXML", "SharedLib")
	includedirs { "../expat" }
	defines { "wxUSE_GUI=0" }

	local xml_deps = { "expat", "wxBase" }
	dependson(xml_deps)
	links(xml_deps)


-- wxWidgetsProject("wxAdv", "SharedLib")
-- 	local adv_deps = { "wxBase", "wxCore" }
-- 	dependson(adv_deps)
-- 	links(adv_deps)

-- 	filter { "system:windows" }
-- 		links { "Winmm" }

-- 	filter {}

wxWidgetsProject("wxAui", "SharedLib")
	local aui_deps = { "wxBase", "wxCore" }
	dependson(aui_deps)
	links(aui_deps)


wxWidgetsProject("wxHTML", "SharedLib")
	local html_deps = { "wxBase", "wxCore" }
	dependson(html_deps)
	links(html_deps)

wxWidgetsProject("wxMedia", "SharedLib")
	local media_deps = { "wxBase", "wxCore" }
	dependson(media_deps)
	links(media_deps)

-- wxWidgetsProject("wxNet", "SharedLib")

wxWidgetsProject("wxPropGrid", "SharedLib")
	local propgrid_deps = { "wxBase", "wxCore" }
	dependson(propgrid_deps)
	links(propgrid_deps)

wxWidgetsProject("wxQA", "SharedLib")
	local qa_deps = { "wxBase", "wxCore", "wxXML" }
	dependson(qa_deps)
	links(qa_deps)

wxWidgetsProject("wxRegex", "StaticLib")

wxWidgetsProject("wxRibbon", "SharedLib")
	local ribbon_deps = { "wxBase", "wxCore" }
	dependson(ribbon_deps)
	links(ribbon_deps)

wxWidgetsProject("wxRichText", "SharedLib")
	local richtext_deps = { "wxBase", "wxCore", "wxHTML", "wxXML", "wxXRC" }
	dependson(richtext_deps)
	links(richtext_deps)

wxWidgetsProject("wxWebview", "SharedLib")
	local webview_deps = { "wxBase", "wxCore" }
	dependson(webview_deps)
	links(webview_deps)

wxWidgetsProject("wxXRC", "SharedLib")
	local xrc_deps = { "wxBase", "wxCore", "wxHTML", "wxXML" }
	dependson(xrc_deps)
	links(xrc_deps)


group "Dependencies"
