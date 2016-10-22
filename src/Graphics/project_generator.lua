group "Graphics Modules"

function SuffixSetup(configuration)
	filter { "configurations:Debug*", "platforms:x86" }
		targetsuffix("_" .. configuration .. "32d")

	filter { "configurations:Release*", "platforms:x86" }
		targetsuffix("_" .. configuration .. "32")

	filter { "configurations:Analyze*", "platforms:x86" }
		targetsuffix("_" .. configuration .. "32a")

	filter { "configurations:Profile*", "platforms:x86" }
		targetsuffix("_" .. configuration .. "32p")

	filter { "configurations:Optimized_Debug*", "platforms:x86" }
		targetsuffix("_" .. configuration .. "32od")

	filter { "configurations:Debug*", "platforms:x64" }
		targetsuffix("_" .. configuration .. "64d")

	filter { "configurations:Release*", "platforms:x64" }
		targetsuffix("_" .. configuration .. "64")

	filter { "configurations:Analyze*", "platforms:x64" }
		targetsuffix("_" .. configuration .. "64a")

	filter { "configurations:Profile*", "platforms:x64" }
		targetsuffix("_" .. configuration .. "64p")

	filter { "configurations:Optimized_Debug*", "platforms:x64" }
		targetsuffix("_" .. configuration .. "64od")

	filter {}
end

function GraphicsCommonSetup(project_name)
	project(project_name)
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/graphics")
		end

		filter { "system:windows" }
			includedirs { "../../dependencies/dirent" }

		filter {}

		kind "SharedLib"
		language "C++"

		flags { "FatalWarnings" }

		files { "**.h", "**.cpp", "**.inl" }

		includedirs
		{
			"../Shared/include",
			"../Memory/include",
			"../../frameworks/Gaff/include",
			"../../frameworks/Gleam/include",
			"../../dependencies/rapidjson"
		}

		dependson
		{
			"Shared", "Gaff",
			"Gleam", "glew",
			"Memory"
		}

		links
		{
			"Shared", "Gaff",
			"Gleam", "glew",
			"Memory"
		}
end


if os.get() == "windows" then
	GraphicsCommonSetup("Graphics_Direct3D")
		defines { "USE_DX" }
		links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		SuffixSetup("Direct3D")
end

GraphicsCommonSetup("Graphics_OpenGL")
	defines { "GLEW_STATIC" }

	filter { "system:windows" }
		links { "OpenGL32" }

	filter { "system:not windows" }
		links { "gl" }

	SuffixSetup("OpenGL")
