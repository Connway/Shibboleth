group "Common"

project "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/shared")
	end

	kind "StaticLib"
	language "C++"

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/glm"
		}
