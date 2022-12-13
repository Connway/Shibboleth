if not _OPTIONS["preproc-pipeline"] then
	Group "Tools/Unused"
end

ToolProject("Preprocessor", "ConsoleApp", true)
	debugdir "../../../workingdir/tools"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../Frameworks/Gaff/include",
		-- "../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/fmt/include",
		"../../Dependencies/argparse"
	}

	local deps =
	{
		"Gaff",
		"EASTL"
	}

	dependson(deps)
	links(deps)

	defines { "FMT_HEADER_ONLY" }

if not _OPTIONS["preproc-pipeline"] then
	Group "Tools"
end
