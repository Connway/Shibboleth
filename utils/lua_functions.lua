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


function GleamRendererDefines()
	filter { "system:windows", "options:not renderer or renderer=d3d11" }
		defines { "GLEAM_USE_D3D11" }

	filter { "system:windows", "options:renderer=d3d12" }
		defines { "GLEAM_USE_D3D12" }

	filter { "system:windows", "options:renderer=vulkan" }
		defines { "GLEAM_USE_VULKAN" }

	filter { "system:linux" }
		defines { "GLEAM_USE_VULKAN" }

	filter { "system:macosx" }
		defines { "GLEAM_USE_METAL" }

	filter {}
end
