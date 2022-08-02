-- $TODO: Make tests like other projects.
local source_dir = GetTestsSourceDirectory()
local base_dir = GetTestsDirectory()

local tests = {
	{
		name = "AllocatorTest",

		includedirs =
		{
			base_dir .. "../Dependencies/EASTL/include",

			base_dir .. "../Frameworks/Gaff/include",
			source_dir .. "../Engine/Engine/include",
			source_dir .. "../Engine/Memory/include"
		},

		links =
		{
			"Engine",
			"EASTL",
			"Memory",
			"Gaff",
			"Gleam",
			"mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter { "system:linux" }
				links { "pthread", "dl" }

			filter {}
		end
	},
	{
		name = "ReflectionTest",

		includedirs =
		{
			base_dir .. "../Dependencies/EASTL/include",
			base_dir .. "../Dependencies/mpack",
			base_dir .. "../Dependencies/rapidjson",

			base_dir .. "../Frameworks/Gaff/include",
			source_dir .. "../Engine/Engine/include",
			source_dir .. "../Engine/Memory/include"
		},

		links =
		{
			"Engine",
			"EASTL",
			"Memory",
			"Gaff",
			"Gleam",
			"mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter { "system:linux" }
				links { "pthread", "dl" }

			filter {}
		end
	},
	{
		name = "ECSTest",

		includedirs =
		{
			base_dir .. "../Dependencies/EASTL/include",
			base_dir .. "../Dependencies/rapidjson",
			base_dir .. "../Dependencies/glm",
			base_dir .. "../Dependencies/mpack",

			base_dir .. "../Frameworks/Gaff/include",
			base_dir .. "../Frameworks/Gleam/include",
			source_dir .. "../Engine/Engine/include",
			source_dir .. "../Engine/Memory/include",

			source_dir .. "../Modules/Resource/include",
			source_dir .. "../Modules/ECS/include"
		},

		links =
		{
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
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter { "system:linux" }
				links { "pthread", "dl" }

			filter {}
		end
	}
}

function GenTest(settings)
	TestProject(settings.name)
		debugdir "../../workingdir/tests"
		language "C++"

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "../Dependencies/Catch2"
		}

		links
		{
			"Catch2"
		}

		SetupConfigMap()

		postbuildcommands
		{
			"{MKDIR} ../../../../../../workingdir/tests",
			"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/tests"
		}

		if settings.files then
			files(settings.files)
		else
			files { base_dir .. settings.name .. ".cpp" }
		end

		if settings.dependson then
			dependson(settings.dependson)
		else
			dependson(settings.links)
		end

		if settings.defines then
			defines(settings.defines)
		end

		if settings.extra then
			settings.extra()
		end

		includedirs(settings.includedirs)
		links(settings.links)
end

table.foreachi(tests, GenTest)
