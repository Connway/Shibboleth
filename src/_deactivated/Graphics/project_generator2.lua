group "Graphics Modules"

function GraphicsCommonSetup(project_name)
	project(project_name)
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/graphics")
		end

		filter { "configurations:not Analyze*" }
			flags { "FatalWarnings" }

		filter { "system:windows" }
			includedirs { "../../dependencies/dirent" }

		filter {}

		kind "SharedLib"
		language "C++"

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
end

GraphicsCommonSetup("Graphics_OpenGL")
	defines { "GLEW_STATIC" }

	filter { "system:windows" }
		links { "OpenGL32" }

	filter { "system:not windows" }
		links { "gl" }
