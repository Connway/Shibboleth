if _OPTIONS["no-luajit"] then
	return
end

DepProject "LuaJIT"
	kind "Makefile"

	language "C"

	files { "**.h", "**.hpp", "**.c", "**.lua", "**.dasc" }
	excludes { "src/jit/vmdef.lua" }

	local linux_make_cmd = "make"

	if _OPTIONS["cc"] then
		linux_make_cmd = "make CC=" .. _OPTIONS["cc"]
	end

	print(_OPTIONS["cc"])

	filter { "system:windows", "platforms:x64", "configurations:*Debug*", "configurations:not Optimized_Debug*" }

		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src",
			"msvcbuild.bat debug static & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.lib %{cfg.targetdir}/lua51.lib & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.pdb %{cfg.targetdir}/lua51.pdb"
		}

	filter { "system:windows", "platforms:x64", "configurations:not *Debug* or Optimized_Debug" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src",
			"msvcbuild.bat static & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.lib %{cfg.targetdir}/lua51.lib"
		}

	filter { "system:linux", "platforms:x64", "configurations:*Debug*", "configurations:not Optimized_Debug*" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src",
			linux_make_cmd .. " CCDEBUG=-g",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.a %{cfg.targetdir}/lua51.a",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.so %{cfg.targetdir}/lua51.so"
		}

	filter { "system:linux", "platforms:x64", "configurations:not *Debug* or Optimized_Debug" }
		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src",
			"msvcbuild.bat debug static",
			"{COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.a %{cfg.targetdir}/lua51.a"
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
			"{DELETE} %{cfg.targetdir}/*.pdb",
		}

	filter { "system:linux", "platforms:x64" }
		cleancommands
		{
		}

	filter {}

	SetupConfigMap()
