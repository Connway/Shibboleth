EngineProject "Engine"
	language "C++"

	local source_dir = GetEngineSourceDirectory("Engine")
	local base_dir = GetEngineDirectory("Engine")

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

	IncludeDirs
	{
		source_dir .. "include",
		source_dir .. "../../Engine/Memory/include",
		base_dir .. "../../Dependencies/rapidjson",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Frameworks/Gleam/include",
		base_dir .. "../../Dependencies/EASTL/include",
		base_dir .. "../../Dependencies/mpack",
		base_dir .. "../../Dependencies/glm",
		base_dir .. "../../Dependencies/zlib-ng",
		base_dir .. "../../Dependencies/libpng",
		base_dir .. "../../Dependencies/libtiff"
	}

	filter { "configurations:*Static*" }
		defines { "SHIB_STATIC" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	StaticHeaderGen()
	SetupConfigMap()
