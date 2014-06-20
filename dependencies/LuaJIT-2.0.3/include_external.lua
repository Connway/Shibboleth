external "LuaJIT"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "SharedLib"
	language "C"
