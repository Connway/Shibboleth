project "mpack"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	-- warnings "Default"

	files { "**.c", "**.h", "**.inl", "**.sample" }

	defines { "MPACK_HAS_CONFIG" }
