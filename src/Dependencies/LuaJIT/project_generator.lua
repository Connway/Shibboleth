DepProject "LuaJIT"
	kind "Makefile"

	language "C"

	files { "**.h", "**.c", "**.lua", "**.dasc" }

	filter { "system:windows", "platforms:x64" }

		buildcommands
		{
			"{CHDIR} ../../../../../src/Dependencies/LuaJIT/src",
			"msvcbuild.bat & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.lib %{cfg.targetdir}/lua51.lib & {COPYFILE} ../../../../../src/Dependencies/LuaJIT/src/lua51.dll %{cfg.targetdir}/lua51.dll"
		}

		cleancommands
		{
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/buildvm.exp",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/buildvm.lib",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/lua51.dll",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/lua51.exp",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/lua51.lib",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/luajit.exe",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/luajit.exp",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/luajit.lib",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/minilua.exp",
			"{DELETE} ../../../../../src/Dependencies/LuaJIT/src/minilua.lib",
			"{DELETE} %{cfg.targetdir}/lua51.lib",
			"{DELETE} %{cfg.targetdir}/lua51.dll"
		}

	filter {}

	SetupConfigMap()
