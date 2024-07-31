CoreProject "Engine"
	language "C++"

	local source_dir = GetCoreSourceDirectory("Engine")
	local base_dir = GetCoreDirectory("Engine")

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

	IncludeDirs
	{
		source_dir,
		source_dir .. "include",
		source_dir .. "../../Core/Memory/include",
		base_dir .. "../../Dependencies/rapidjson",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Frameworks/Gleam/include",
		base_dir .. "../../Dependencies/EASTL/include",
		base_dir .. "../../Dependencies/mpack",
		base_dir .. "../../Dependencies/glm",
		base_dir .. "../../Dependencies/zlib-ng",
		base_dir .. "../../Dependencies/libtiff/libtiff",
		base_dir .. "../../Dependencies/libpng"
	}

	filter { "configurations:*Static*" }
		defines { "SHIB_STATIC" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	StaticHeaderGen()
	SetupConfigMap()
