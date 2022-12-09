--local source_dir = GetTestsSourceDirectory("ECSTest")
local source_dir = GetEngineSourceDirectory("Engine")
local base_dir = GetTestsDirectory("ECSTest")

TestProject "ECSTest"
	debugdir "../../workingdir/tests"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.cpp", "**.h" }
	files { "../Shibboleth_TestMain.cpp" }

	IncludeDirs
	{
		base_dir .. "../../Dependencies/Catch2",
		base_dir .. "../../Dependencies/EASTL/include",
		base_dir .. "../../Dependencies/glm",
		base_dir .. "../../Dependencies/rapidjson",
		base_dir .. "../../Dependencies/mpack",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Frameworks/Gleam/include",
		source_dir .. "../../Engine/Engine/include",
		source_dir .. "../../Engine/Memory/include",
		source_dir .. "../../Modules/Resource/include",
		source_dir .. "../../Modules/ECS/include"
	}

	local deps =
	{
		"Catch2",
		"Engine",
		"EASTL",
		"Memory",
		"Gaff",
		"Gleam",
		"mpack",
		"ECS",
		"Resource",
		"MainLoop",
		"DevDebug"
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
