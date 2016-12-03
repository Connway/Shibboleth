project "nana"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"
	warnings "Default"

	files { "**.cpp", "**.h", "**.inl" }
	includedirs { "include" }
