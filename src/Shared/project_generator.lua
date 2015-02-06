project "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/shared")
	end

	kind "StaticLib"
	language "C++"

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../../dependencies/jansson",
		"../../dependencies/Gaff/include",
		"../../dependencies/utf8-cpp"
	}
