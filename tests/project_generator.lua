local tests = {
	{
		name = "AllocatorTest",

		includedirs = {
			"../dependencies/EASTL/include",
			"../dependencies/CATCH",

			"../frameworks/Gaff/include",
			"../src/Shared/include",
			"../src/Memory/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared"
		}
	},
	{
		name = "ReflectionTest",

		includedirs = {
			"../dependencies/EASTL/include",
			"../dependencies/CATCH",
			"../dependencies/libuv/include",
			"../dependencies/mpack",
			"../dependencies/rapidjson",

			"../frameworks/Gaff/include",
			"../src/Shared/include",
			"../src/Memory/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared",
			"libuv", "mpack"
		},

		extra = function ()
			filter { "system:windows" }
				links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib", "DbgHelp" }

			filter {}
		end
	},
	{
		name = "ScriptTest",

		includedirs = {
			"../dependencies/EASTL/include",
			"../dependencies/CATCH",
			"../dependencies/libuv/include",
			"../dependencies/angelscript/angelscript/include",
			"../dependencies/angelscript/add_on/scriptbuilder",

			"../frameworks/Gaff/include",
			"../src/Shared/include",
			"../src/Memory/include",
			"../src/Modules/Scripting/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared",
			"angelscript", "libuv",
			"Scripting", "Entity",
			"Resource", "Gleam"
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
			location ("../project/" .. _ACTION .. "/tests")
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
