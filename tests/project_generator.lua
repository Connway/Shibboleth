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

			"../frameworks/Gaff/include",
			"../src/Shared/include",
			"../src/Memory/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }

			filter {}
		end
	},
	{
		name = "ScriptTest",

		includedirs = {
			"../dependencies/EASTL/include",
			"../dependencies/CATCH",
			"../dependencies/chaiscript",
			"../dependencies/angelscript/angelscript/include",
			"../dependencies/angelscript/add_on/scriptbuilder",

			"../frameworks/Gaff/include",
			"../src/Shared/include",
			"../src/Memory/include",
			"../src/Scripting/include"
		},

		links = {
			"Gaff", "Memory",
			"EASTL", "Shared",
			"Scripting", "angelscript"
		},

		extra = function ()
			filter { "system:windows" }
				links { "DbgHelp" }
				-- buildoptions { "/bigobj"}

			filter {}
		end
	}
}

for i = 1, table.getn(tests) do
	local settings = tests[i]

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
