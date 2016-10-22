group "Messages Library"

project "Messages"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/messages")
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

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
