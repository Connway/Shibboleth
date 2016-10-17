if _OPTIONS["physx"] then

	project "PhysX"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/dependencies")
		end

		dofile("../../utils/default_configs.lua")
		dofile("../../utils/config_map.lua")

		kind "Makefile"

		-- VS2013
		filter { "configurations:Debug", "platforms:x86", "action:vs2013" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32/PhysX.sln /t:Build /p:Configuration=debug /p:Platform=Win32"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32/PhysX.sln /t:Rebuild /p:Configuration=debug /p:Platform=Win32"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32/PhysX.sln /t:Clean /p:Configuration=debug /p:Platform=Win32"
			}

		filter { "configurations:Release", "platforms:x86", "action:vs2013" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32/PhysX.sln /t:Build /p:Configuration=release /p:Platform=Win32"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32/PhysX.sln /t:Rebuild /p:Configuration=release /p:Platform=Win32"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win32/PhysX.sln /t:Clean /p:Configuration=release /p:Platform=Win32"
			}

		filter { "configurations:Debug", "platforms:x64", "action:vs2013" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64/PhysX.sln /t:Build /p:Configuration=debug /p:Platform=x64"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64/PhysX.sln /t:Rebuild /p:Configuration=debug /p:Platform=x64"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64/PhysX.sln /t:Clean /p:Configuration=debug /p:Platform=x64"
			}

		filter { "configurations:Release", "platforms:x64", "action:vs2013" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64/PhysX.sln /t:Build /p:Configuration=release /p:Platform=x64"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64/PhysX.sln /t:Rebuild /p:Configuration=release /p:Platform=x64"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc12win64/PhysX.sln /t:Clean /p:Configuration=release /p:Platform=x64"
			}


		-- VS2015
		filter { "configurations:Debug", "platforms:x86", "action:vs2015" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32/PhysX.sln /t:Build /p:Configuration=debug /p:Platform=Win32"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32/PhysX.sln /t:Rebuild /p:Configuration=debug /p:Platform=Win32"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32/PhysX.sln /t:Clean /p:Configuration=debug /p:Platform=Win32"
			}

		filter { "configurations:Release", "platforms:x86", "action:vs2015" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32/PhysX.sln /t:Build /p:Configuration=release /p:Platform=Win32"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32/PhysX.sln /t:Rebuild /p:Configuration=release /p:Platform=Win32"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win32/PhysX.sln /t:Clean /p:Configuration=release /p:Platform=Win32"
			}

		filter { "configurations:Debug", "platforms:x64", "action:vs2015" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64/PhysX.sln /t:Build /p:Configuration=debug /p:Platform=x64"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64/PhysX.sln /t:Rebuild /p:Configuration=debug /p:Platform=x64"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64/PhysX.sln /t:Clean /p:Configuration=debug /p:Platform=x64"
			}

		filter { "configurations:Release", "platforms:x64", "action:vs2015" }
			buildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64/PhysX.sln /t:Build /p:Configuration=release /p:Platform=x64"
			}

			rebuildcommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64/PhysX.sln /t:Rebuild /p:Configuration=release /p:Platform=x64"
			}

			cleancommands
			{
				"msbuild ../../../dependencies/PhysX/PhysXSDK/Source/compiler/vc14win64/PhysX.sln /t:Clean /p:Configuration=release /p:Platform=x64"
			}

		filter {}

end
