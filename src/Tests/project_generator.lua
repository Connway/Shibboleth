local tests = {
	{
		name = "AllocatorTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",
			"../Dependencies/optick",

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
			"mpack",
			"optick"
		},

		extra = function ()
			filter { "system:windows" }
				-- links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }
				links { "DbgHelp" }

			-- filter { "action:vs*" }
			-- 	buildoptions { "/bigobj" }

			filter {}
		end
	},
	{
		name = "ReflectionTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",
			"../Dependencies/mpack",
			"../Dependencies/optick",
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
			"mpack",
			"optick"
		},

		extra = function ()
			filter { "system:windows" }
				-- links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }
				links { "DbgHelp" }

			-- filter { "action:vs*" }
			-- 	buildoptions { "/bigobj" }

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
			"../Dependencies/optick",

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
			"mpack",
			"optick",
		},

		extra = function ()
			filter { "system:windows" }
				-- links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }
				links { "DbgHelp" }

			-- filter { "action:vs*" }
			-- 	buildoptions { "/bigobj" }

			filter {}
		end
	}
}

function GenTest(settings)
	project(settings.name)
		if _ACTION then
			location(GetTestsLocation())
		end

		kind(settings.kind or "ConsoleApp")
		debugdir "../../workingdir/tests"
		language "C++"

		flags { "FatalWarnings" }

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
end

table.foreachi(tests, GenTest)
