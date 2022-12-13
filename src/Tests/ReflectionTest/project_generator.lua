--local source_dir = GetTestsSourceDirectory("ReflectionTest")
local source_dir = GetEngineSourceDirectory("Engine")
local base_dir = GetTestsDirectory("ReflectionTest")

TestProject "ReflectionTest"
	debugdir "../../workingdir/tests"
	language "C++"

	files { "**.cpp", "**.h" }
	files { "../Shibboleth_TestMain.cpp" }

	IncludeDirs
	{
		base_dir .. "../../Dependencies/Catch2",
		base_dir .. "../../Dependencies/EASTL/include",
		base_dir .. "../../Dependencies/rapidjson",
		base_dir .. "../../Dependencies/mpack",
		base_dir .. "../../Frameworks/Gaff/include",
		source_dir .. "../../Engine/Engine/include",
		source_dir .. "../../Engine/Memory/include"
	}

	local deps =
	{
		"Catch2",
		"Engine",
		"EASTL",
		"Memory",
		"Gaff",
		"Gleam",
		"mpack"
	}

	dependson(deps)
	links(deps)

	filter { "system:windows" }
		links { "DbgHelp" }

	filter { "system:linux" }
		links { "pthread", "dl" }

	filter {}
