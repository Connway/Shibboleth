local tests = {
	{
		name = "AllocatorTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",

			"../Frameworks/Gaff/include",
			"../Engine/Engine/include",
			"../Engine/Memory/include"
		},

		links =
		{
			"Gaff",
			"Gleam",
			"Memory",
			"EASTL",
			"Engine",
			"mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter {}
		end
	},
	{
		name = "ReflectionTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",
			"../Dependencies/mpack",
			"../Dependencies/rapidjson",

			"../Frameworks/Gaff/include",
			"../Engine/Engine/include",
			"../Engine/Memory/include"
		},

		links =
		{
			"Gaff",
			"Gleam",
			"Memory",
			"EASTL",
			"Engine",
			"mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter {}
		end
	},
	{
		name = "ECSTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",
			"../Dependencies/rapidjson",
			"../Dependencies/glm",
			"../Dependencies/mpack",

			"../Frameworks/Gaff/include",
			"../Frameworks/Gleam/include",
			"../Engine/Engine/include",
			"../Engine/Memory/include",

			"../Modules/Resource/include",
			"../Modules/ECS/include"
		},

		links =
		{
			"Gaff",
			"Gleam",
			"Memory",
			"Engine",

			"ECS",
			"Resource",
			"MainLoop",

			"EASTL",
			"mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter {}
		end
	}
}

function GenTest(settings)
	project(settings.name)
		location(GetTestsLocation())

		kind(settings.kind or "ConsoleApp")
		debugdir "../../workingdir/tests"
		language "C++"

		flags { "FatalWarnings" }

		includedirs
		{
			"../Dependencies/Catch2"
		}

		links
		{
			"Catch2"
		}

		SetupConfigMap()

		ModuleCopy("tests")


		if settings.files then
			files(settings.files)
		else
			files { settings.name .. ".cpp" }
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
