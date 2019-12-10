local tests = {
	{
		name = "AllocatorTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",
			"../Dependencies/doctest",

			"../Frameworks/Gaff/include",
			"../Engine/Engine/include",
			"../Engine/Memory/include"
		},

		links =
		{
			"Gaff", "Memory",
			"EASTL", "Engine"
		}
	},
	{
		name = "ReflectionTest",

		includedirs =
		{
			"../Dependencies/EASTL/include",
			"../Dependencies/doctest",
			"../Dependencies/mpack",
			"../Dependencies/rapidjson",

			"../Frameworks/Gaff/include",
			"../Engine/Engine/include",
			"../Engine/Memory/include"
		},

		links =
		{
			"Gaff", "Memory",
			"EASTL", "Engine",
			"mpack"
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
			"../Dependencies/doctest",
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
			"Gaff", "Gleam",
			"Memory", "EASTL",
			"Engine", "ECS",
			"mpack", "Resource"
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
	-- {
	-- 	name = "ScriptTest",

	-- 	includedirs =
	-- {
	-- 		"../Dependencies/EASTL/include",
	-- 		"../Dependencies/doctest",
	-- 		"../Dependencies/angelscript/angelscript/include",
	-- 		"../Dependencies/angelscript/add_on/scriptbuilder",
	-- 		"../Dependencies/rapidjson",

	-- 		"../Frameworks/Gaff/include",
	-- 		"../Engine/Engine/include",
	-- 		"../Engine/Memory/include",
	-- 		"../Modules/Scripting/include"
	-- 	},

	-- 	links =
	-- {
	-- 		"Gaff", "Memory",
	-- 		"EASTL", "Engine",
	-- 		"Scripting", "Entity",
	-- 		"Resource", "Gleam",
	-- 		"angelscript"
	-- 	},

	-- 	extra = function ()
	-- 		filter { "system:windows" }
	-- 			links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }

	-- 		filter {}
	-- 	end
	-- }
}

function GenTest(settings)
	project(settings.name)
		if _ACTION then
			location(GetTestsLocation())
		end

		kind "ConsoleApp"
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

		if settings.extra then
			settings.extra()
		end

		includedirs(settings.includedirs)
		links(settings.links)

		SetupConfigMap()
end

table.foreachi(tests, GenTest)
