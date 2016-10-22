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

		flags { "FatalWarnings" }

		if tests.files == nil then
			files { settings.name .. ".cpp" }
		else
			files(settings.files)
		end

		if tests.dependson == nil then
			dependson(settings.links)
		else
			dependson(settings.dependson)
		end

		includedirs(settings.includedirs)
		links(settings.links)

end
