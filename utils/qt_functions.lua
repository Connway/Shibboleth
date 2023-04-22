require("premake-qt/qt")
local qt = premake.extensions.qt

function QtSettingsModule(modules, base_dir, source_dir)
	if base_dir == nil then
		base_dir = ""
	end

	if source_dir == nil then
		source_dir = base_dir
	end

	defines { "QT_DISABLE_DEPRECATED_BEFORE=0x060000" }

	-- Doing this instead of "%{cfg.objdir}/qt", as that is not behaving consistently amongst all project generators.
	qtqmgenerateddir(GetIntermediateLocation() .. "/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/qt")
	qtgenerateddir(GetIntermediateLocation() .. "/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}/qt")
	qtprefix "Qt6"

	qtmodules(modules)

	filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Debug", "platforms:x64 or arm64" }
		qtsuffix "d"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Release", "platforms:x64 or arm64" }
	-- 	qtsuffix "64"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Profile", "platforms:x64 or arm64" }
	-- 	qtsuffix "64p"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Optimized_Debug", "platforms:x64 or arm64" }
	-- 	qtsuffix "64od"

	filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Static_Debug*", "platforms:x64 or arm64" }
		qtsuffix "d"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Static_Release*", "platforms:x64 or arm64" }
	-- 	qtsuffix "64s"

	filter {}
end

function QtSettings(modules, base_dir, source_dir)
	if base_dir == nil then
		base_dir = ""
	end

	if source_dir == nil then
		source_dir = base_dir
	end

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.qrc", source_dir .. "**.ui" }

	QtSettingsModule(modules, base_dir, source_dir)
end

function CopyQtFilesToBin(qt_modules)
	local plugin_path = qt.defaultpath .. "/plugins"
	local bin_path = qt.defaultpath .. "/bin"
	local extension = GetSharedLibExtension()

	postbuildcommands
	{
		"{MKDIR} " .. GetBinLocation(),
		"{MKDIR} " .. GetBinLocation() .. "/platforms",
		"{COPYFILE} " .. plugin_path .. "/platforms/qwindowsd" .. extension .. " " .. GetBinLocation() .. "/platforms",
		"{COPYFILE} " .. plugin_path .. "/platforms/qwindows" .. extension .. " " .. GetBinLocation() .. "/platforms"
	}

	for _, name in ipairs(qt_modules) do
		local final_name = bin_path .. "/Qt6" .. qt.modules.qt6[name].name

		postbuildcommands
		{
			"{COPYFILE} " .. final_name .. "d" .. extension .. " " .. GetBinLocation(),
			"{COPYFILE} " .. final_name .. extension .. " " .. GetBinLocation()
		}
	end
end
