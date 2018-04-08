local tests = {
	{
		name = "AllocatorTest",

		includedirs = {
			"../Dependencies/EASTL/include",
			"../Dependencies/CATCH",

			"../Frameworks/Gaff/include",
			"../Engine/Shared/include",
			"../Engine/Memory/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared"
		}
	},
	{
		name = "ReflectionTest",

		includedirs = {
			"../Dependencies/EASTL/include",
			"../Dependencies/CATCH",
			"../Dependencies/mpack",
			"../Dependencies/rapidjson",

			"../Frameworks/Gaff/include",
			"../Engine/Shared/include",
			"../Engine/Memory/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared",
			"mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }

			filter { "action:vs*" }
				buildoptions { "/bigobj" }

			filter {}
		end
	},
	{
		name = "ScriptTest",

		includedirs = {
			"../Dependencies/EASTL/include",
			"../Dependencies/CATCH",
			"../Dependencies/angelscript/angelscript/include",
			"../Dependencies/angelscript/add_on/scriptbuilder",

			"../Frameworks/Gaff/include",
			"../Engine/Shared/include",
			"../Engine/Memory/include",
			"../Modules/Scripting/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared",
			"Scripting", "Entity",
			"Resource", "Gleam",
			"angelscript"
		},

		extra = function ()
			filter { "system:windows" }
				links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }

			filter {}
		end
	}
}

function GenTest(settings)
	project(settings.name)
		if _ACTION then
			location ("../../.generated/project/" .. _ACTION .. "/tests")
		end

		kind "ConsoleApp"
		language "C++"

		filter { "configurations:not Analyze*" }
			flags { "FatalWarnings" }

		filter {}

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
end

table.foreachi(tests, GenTest)
