project "Gaff"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not *Clang", "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "configurations:Debug* or Optimized_Debug*" }
		defines { "ENET_DEBUG" }

	filter {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../dependencies/EASTL/include",
		"../../dependencies/enet/include",
		"../../dependencies/rapidjson",
		"../../dependencies/mpack"
	}
