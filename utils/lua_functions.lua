function GetLuaIncDep()
	if _OPTIONS["no-luajit"] then
		return "lua"
	else
		return "LuaJIT/src"
	end
end

function GetLuaDepDep()
	if _OPTIONS["no-luajit"] then
		return "Lua"
	else
		return "LuaJIT"
	end
end

function GetLuaLinkDep()
	if _OPTIONS["no-luajit"] then
		return "Lua"

	else
		if os.target() == "windows" then
			return "%{cfg.targetdir}/../LuaJIT/lua51.lib"
		elseif os.target() == "linux" then
			return "%{cfg.targetdir}/../LuaJIT/lua51.a"
		elseif os.target() == "macosx" then
			return "%{cfg.targetdir}/../LuaJIT/lua51.a"
		end
	end
end
