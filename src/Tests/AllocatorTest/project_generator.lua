local source_dir = GetTestsSourceDirectory("AllocatorTest")
local base_dir = GetTestsDirectory("AllocatorTest")

TestProject "AllocatorTest"
	debugdir "../../workingdir/tests"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.cpp", "**.h" }

	includedirs
	{
		base_dir .. "../Dependencies/Catch2",
		base_dir .. "../Dependencies/EASTL/include",
		base_dir .. "../Frameworks/Gaff/include",
		source_dir .. "../Engine/Engine/include",
		source_dir .. "../Engine/Memory/include"
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

	SetupConfigMap()
	TestCopy()

	filter { "system:windows" }
		links { "DbgHelp" }

	filter { "system:linux" }
		links { "pthread", "dl" }

	filter {}
