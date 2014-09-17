project "Gleam"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	defines { "GLEW_STATIC" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../glew/include",
		"../utf8-cpp"
	}
