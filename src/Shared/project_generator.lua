project "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION)
	end

	kind "StaticLib"
	language "C++"

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../dependencies/jansson",
		"../../dependencies/Gaff/include",
		"../../dependencies/utf8-cpp"
	}
