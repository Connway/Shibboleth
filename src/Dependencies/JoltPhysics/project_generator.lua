DepProject "JoltPhysics"
	language "C++"

	files { "Jolt/**.h", "Jolt/**.cpp", "Jolt/**.natvis" }
	includedirs { "." }

	--filter { "action:not vs*", "toolset:gcc or clang" }
	--	buildoptions { "-fPIC" }

	--filter {}

	SetupConfigMap()

if os.target() == "windows" then
	DepProject("JoltViewer", "WindowedApp")
		language "C++"
		debugdir "../../../workingdir/tools/JoltViewer"

		files
		{
			"JoltViewer/**.h",
			"JoltViewer/**.cpp",
			--"Assets/**.hlsl",
			"TestFramework/**.h",
			"TestFramework/**.cpp"
		}

		includedirs { ".", "JoltViewer", "TestFramework" }

		dependson {"JoltPhysics" }
		links
		{
			"JoltPhysics",
			"d3d12.lib",
			"shcore.lib",
			"dxguid.lib",
			"dinput8.lib",
			"dxgi.lib",
			"d3dcompiler.lib"
		}

		postbuildcommands
		{
			"{MKDIR} ../../../../../workingdir/tools",
			"{MKDIR} ../../../../../workingdir/tools/JoltViewer",
			"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools/JoltViewer",
			"{COPYDIR} ../../../../../src/Dependencies/JoltPhysics/Assets ../../../../../workingdir/tools/JoltViewer/Assets",
		}

		--[[shaderoptions { "/WX" }
		shadermodel "5.0"

		filter { "files:**/*VertexShader*.hlsl"}
			shadertype "Vertex"

		filter { "files:**/*PixelShader*.hlsl"}
			shadertype "Pixel"--]]

		--filter { "action:not vs*", "toolset:gcc or clang" }
		--	buildoptions { "-fPIC" }

		--filter {}

		SetupConfigMap()
end
