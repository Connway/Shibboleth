group "Messages Library"

project "Messages"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/messages")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../../frameworks/Gaff/include",
		"../../dependencies/rapidjson/",
		"../../dependencies/utf8-cpp"
	}
