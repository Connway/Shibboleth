group "Graphics Modules"

project "Graphics"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/graphics")
	end

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

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
		"../../dependencies/rapidjson",
		"../../dependencies/utf8-cpp"
	}

	dependson
	{
		"Shared", "Gaff",
		"Gleam", "glew",
		"Memory", "Boxer"
	}

	links
	{
		"Shared", "Gaff",
		"Gleam", "glew",
		"Memory", "Boxer"
	}

	filter { "configurations:*Direct3D"}
		defines { "USE_DX" }

	filter { "configurations:*Analyze"}
		defines { "USE_DX" }

	filter { "configurations:*OpenGL"}
		defines { "GLEW_STATIC" }

	filter { "configurations:Debug_Direct3D", "platforms:x86" }
		targetsuffix "_Direct3D32d"

	filter { "configurations:Release_Direct3D", "platforms:x86" }
		targetsuffix "_Direct3D32"

	filter { "configurations:Debug_OpenGL", "platforms:x86" }
		targetsuffix "_OpenGL32d"

	filter { "configurations:Release_OpenGL", "platforms:x86" }
		targetsuffix "_OpenGL32"

	filter { "configurations:Debug_Direct3D", "platforms:x64" }
		targetsuffix "_Direct3D64d"

	filter { "configurations:Release_Direct3D", "platforms:x64" }
		targetsuffix "_Direct3D64"

	filter { "configurations:Debug_OpenGL", "platforms:x64" }
		targetsuffix "_OpenGL64d"

	filter { "configurations:Release_OpenGL", "platforms:x64" }
		targetsuffix "_OpenGL64"

	-- Just use Direct3D for the Analyze builds.
	filter { "configurations:Debug_Analyze", "platforms:x86" }
		targetsuffix "_Direct3D32d"

	filter { "configurations:Release_Analyze", "platforms:x86" }
		targetsuffix "_Direct3D32"

	filter { "configurations:Debug_Analyze", "platforms:x64" }
		targetsuffix "_Direct3D64d"

	filter { "configurations:Release_Analyze", "platforms:x64" }
		targetsuffix "_Direct3D64"

	filter { "configurations:*OpenGL", "system:windows" }
		links { "OpenGL32" }

	filter { "configurations:*OpenGL", "system:not windows" }
		links { "gl" }

	filter { "configurations:*Direct3D", "system:windows" }
		links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }

	filter {}
