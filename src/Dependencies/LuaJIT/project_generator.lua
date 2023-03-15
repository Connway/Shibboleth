if _OPTIONS["no-luajit"] then
	return
end

DepProject "LuaJIT"
	kind "Makefile"

	language "C"

	files { "**.h", "**.hpp", "**.c", "**.lua", "**.dasc" }
	excludes { "src/jit/vmdef.lua", "project_generator.lua" }

	local linux_make_cmd = "make BUILDMODE=static"

	if _OPTIONS["cc"] then
		linux_make_cmd = "make BUILDMODE=static CC=" .. _OPTIONS["cc"]
	end

	filter { "system:windows", "platforms:x64", "configurations:*Debug*", "configurations:not Optimized_Debug*" }

		buildcommands
		{
			"if EXIST %{cfg.targetdir}/lua51.lib ( {ECHO} LuaJIT already built. ) ELSE ( {CHDIR} ../../../../../src/Dependencies/LuaJIT/src & msvcbuild.bat debug static & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.lib %{cfg.targetdir}/lua51.lib & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.pdb %{cfg.targetdir}/lua51.pdb )"
		}

	filter { "system:windows", "platforms:x64", "configurations:not *Debug* or Optimized_Debug" }
		buildcommands
		{
			"if EXIST %{cfg.targetdir}/lua51.lib ( {ECHO} LuaJIT already built. ) ELSE ( {CHDIR} ../../../../../src/Dependencies/LuaJIT/src & msvcbuild.bat static & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.lib %{cfg.targetdir}/lua51.lib )"
		}

	filter { "system:linux", "platforms:x64", "configurations:*Debug*", "configurations:not Optimized_Debug*" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src && " .. linux_make_cmd .. " CCDEBUG=-g",
			"{MKDIR} %{cfg.targetdir}",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.a %{cfg.targetdir}/libluajit.a",
			--"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.so %{cfg.targetdir}/libluajit.so"
		}

	filter { "system:linux", "platforms:x64", "configurations:not *Debug* or Optimized_Debug" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src && " .. linux_make_cmd,
			"{MKDIR} %{cfg.targetdir}",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.a %{cfg.targetdir}/libluajit.a",
			--"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.so %{cfg.targetdir}/libluajit.so"
		}

		filter { "system:macosx", "platforms:arm64", "configurations:*Debug*", "configurations:not Optimized_Debug*" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src && MACOSX_DEPLOYMENT_TARGET=13.2 " .. linux_make_cmd .. " CCDEBUG=-g",
			"{MKDIR} %{cfg.targetdir}",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.a %{cfg.targetdir}/libluajit.a",
			--"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.dylib %{cfg.targetdir}/libluajit.dylib"
		}

	filter { "system:macosx", "platforms:arm64", "configurations:not *Debug* or Optimized_Debug" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src && MACOSX_DEPLOYMENT_TARGET=13.2 " .. linux_make_cmd,
			"{MKDIR} %{cfg.targetdir}",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.a %{cfg.targetdir}/libluajit.a",
			--"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/libluajit.dylib %{cfg.targetdir}/libluajit.dylib"
		}

	filter { "system:windows", "platforms:x64" }
		cleancommands
		{
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/jit/vmdef.lua",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/*.exe",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/*.exp",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/*.lib",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/*.pdb",
			--"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/*.dll",
			"{DELETE} %{cfg.targetdir}/*.lib",
			"{DELETE} %{cfg.targetdir}/*.pdb"
		}

	filter { "system:linux or macosx", "platforms:x64 or arm64" }
		cleancommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src && make clean"
		}

	filter {}

	SetupConfigMap()
