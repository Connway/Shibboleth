group "Messages Library"

project "Messages"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/messages")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/jansson/",
		"../../dependencies/utf8-cpp"
	}