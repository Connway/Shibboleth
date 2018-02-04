project "nana"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"
	warnings "Default"
	rtti "On"

	files { "**.cpp", "**.h", "**.inl", "**.hpp" }
	includedirs
	{
		"include",
		"../libjpeg",
		"../libpng"
	}
