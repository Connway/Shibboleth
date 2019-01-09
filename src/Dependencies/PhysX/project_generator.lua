function PhysXProject(proj_name, lib_type)
	project(proj_name)

	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind(lib_type)
	language "C++"

	includedirs
	{
		"physx/include",
		"pxshared/include",
		"physx/source/foundation/include"
	}

	dofile(string.lower(proj_name) .. "_files.lua")

	filter { "configurations:Debug*" }
		defines
		{
			"PX_DEBUG=1",
			"PX_CHECKED=1",
			"PX_NVTX=0",
			"PX_SUPPORT_PVD=1",
			-- "PX_BUILD_NUMBER=0"
		}

	filter { "configurations:Release*" }
		defines
		{
			-- "PX_BUILD_NUMBER=0"
			"PX_SUPPORT_PVD=0"
		}

	filter { "action:vs*" }
		defines
		{
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_WINSOCK_DEPRECATED_NO_WARNINGS"
		}

	filter {}

	if lib_type == "SharedLib" then
		postbuildcommands
		{
			"{MKDIR} ../../../../../workingdir/bin",
			"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
		}
	end
end


group "Dependencies/PhysX"

PhysXProject("FastXml", "StaticLib")
	defines
	{
		"PX_FOUNDATION_DLL=0"
	}

	includedirs
	{
		"physx/source/fastxml/include"
	}

PhysXProject("LowLevel", "StaticLib")
	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/ccd",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/lowlevel/api/include",
		"physx/source/lowlevel/common/include/collision",
		"physx/source/lowlevel/common/include/pipeline",
		"physx/source/lowlevel/common/include/utils",
		"physx/source/lowlevel/common/include",
		"physx/source/lowlevel/software/include",
		"physx/source/lowleveldynamics/include",
		"physx/source/physxgpu/include"
	}

	filter { "system:windows" }
		includedirs
		{
			"physx/source/common/include/windows",
			"physx/source/common/src/windows"
		}

	filter {}


PhysXProject("LowLevelAABB", "StaticLib")
	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/lowlevel/api/include",
		"physx/source/lowlevel/common/include/pipeline",
		"physx/source/lowlevel/common/include/utils",
		"physx/source/lowlevel/common/include",
		"physx/source/lowlevelaabb/include",
		"physx/source/lowlevelaabb/src"
	}

	filter { "system:windows" }
		includedirs
		{
			"physx/source/common/include/windows",
			"physx/source/common/src/windows"
		}

	filter {}

PhysXProject("LowLevelDynamics", "StaticLib")
	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src/contact",
		"physx/source/lowlevel/api/include",
		"physx/source/lowlevel/common/include/pipeline",
		"physx/source/lowlevel/common/include/utils",
		"physx/source/lowlevel/common/include",
		"physx/source/lowlevel/software/include",
		"physx/source/lowleveldynamics/include",
		"physx/source/lowleveldynamics/src",
		"physx/source/physxgpu/include"
	}

	filter { "system:windows" }
		includedirs
		{
			"physx/source/common/src/windows"
		}

	filter {}

PhysXProject("PhysXCharacterKinematic", "StaticLib")
	defines { "PX_PHYSX_CHARACTER_STATIC_LIB" }

	includedirs
	{
		"physx/include/common",
		"physx/include/characterkinematic",
		"physx/include/extensions",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm"
	}

PhysXProject("PhysXExtensions", "StaticLib")
	includedirs
	{
		"physx/include/common",
		"physx/include/cooking",
		"physx/include/extensions",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/include/pvd",
		"physx/include/vehicle",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/fastxml/include",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/physxextensions/src",
		"physx/source/physxextensions/src/serialization/Binary",
		"physx/source/physxextensions/src/serialization/File",
		"physx/source/physxextensions/src/serialization/Xml",
		"physx/source/physxmetadata/core/include",
		"physx/source/physxmetadata/extensions/include",
		"physx/source/physx/src",
		"physx/source/pvdsdk/src",
		"physx/source/pvd/include"
	}

PhysXProject("PhysXPvdSDK", "StaticLib")
	defines { "PX_PHYSX_STATIC_LIB" }

	includedirs
	{
		"physx/source/filebuf/include",
		"physx/source/pvd/include"
	}

PhysXProject("PhysXTask", "StaticLib")
	includedirs
	{
		"physx/source/cudamanager/include"
	}

PhysXProject("PhysXVehicle", "StaticLib")
	defines { "PX_PHYSX_STATIC_LIB" }

	includedirs
	{
		"physx/include/common",
		"physx/include/extensions",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/include/physxprofilesdk",
		"physx/include/pvd",
		"physx/include/vehicle",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/physxextensions/src/serialization/Xml",
		"physx/source/physxmetadata/core/include",
		"physx/source/physxmetadata/extensions/include",
		"physx/source/physxvehicle/src/physxmetadata/include",
		"physx/source/physxvehicle/src",
		"physx/source/pvdsdk/src",
		"physx/source/pvd/include"
	}

PhysXProject("SceneQuery", "StaticLib")
	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/include/pvd",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/ccd",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/lowlevel/api/include",
		"physx/source/scenequery/include",
		"physx/source/simulationcontroller/include",
		"physx/source/physx/src/buffering",
		"physx/source/physx/src",
		"physx/source/pvd/include"
	}

PhysXProject("SimulationController", "StaticLib")
	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/include/pvd",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/ccd",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/lowlevel/api/include",
		"physx/source/lowlevel/common/include/collision",
		"physx/source/lowlevel/common/include/pipeline",
		"physx/source/lowlevel/common/include/utils",
		"physx/source/lowlevel/common/include",
		"physx/source/lowlevel/software/include",
		"physx/source/lowlevelaabb/include",
		"physx/source/lowleveldynamics/include",
		"physx/source/physxgpu/include",
		"physx/source/simulationcontroller/include",
		"physx/source/simulationcontroller/src"
	}

PhysXProject("PhysXCommon", "SharedLib")
	defines
	{
		"PX_FOUNDATION_DLL=1",
		"PX_PHYSX_COMMON_EXPORTS"
	}

	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/ccd",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/physxgpu/include"
	}

	filter { "system:not windows" }
		excludes { "physx/source/common/src/windows/*.*" }

	filter {}

	local common_deps =
	{
		"PhysXFoundation"
	}

	dependson(common_deps)
	links(common_deps)

PhysXProject("PhysXCooking", "SharedLib")
	defines
	{
		"PX_COOKING",
		"PX_FOUNDATION_DLL=1",
		"PX_PHYSX_COMMON_EXPORTS",
		"PX_PHYSX_COOKING_EXPORTS",
		"PX_PHYSX_LOADER_EXPORTS",
		"PX_PHYSX_CORE_EXPORTS"
	}

	includedirs
	{
		"physx/include/common",
		"physx/include/cooking",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/ccd",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/physxcooking/src",
		"physx/source/physxcooking/src/convex",
		"physx/source/physxcooking/src/mesh",
		"physx/source/physxgpu/include",
	}

	filter { "system:not windows" }
		excludes { "physx/source/physxcooking/src/windows/*.*"}

	filter {}

	local cooking_deps =
	{
		"PhysXCommon",
		"PhysXFoundation"
	}

	dependson(cooking_deps)
	links(cooking_deps)

PhysXProject("PhysXFoundation", "SharedLib")
	defines
	{
		"PX_FOUNDATION_DLL=1"
	}

	includedirs
	{
		"physx/source/foundation/include",
	}

	filter { "system:windows" }
		includedirs
		{
			"physx/source/foundation/include/windows"
		}

	filter { "system:not windows" }
		excludes { "physx/source/foundation/src/windows/*.*" }
		files { "physx/source/foundation/src/unix/*.*" }

	filter {}


PhysXProject("PhysX", "SharedLib")
	defines
	{
		"PX_FOUNDATION_DLL=1",
		"PX_PHYSX_CORE_EXPORTS",
	}

	filter { "system:windows", "configurations:Debug*" }
		defines { "PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64d.dll" }

		postbuildcommands
		{
			"{MKDIR} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp",
			"{COPY} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/PhysXGpu_64.dll ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp",
			"{MOVE} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp/PhysXGpu_64.dll ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp/PhysXGpu_64d.dll",
			"{COPY} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp/PhysXGpu_64d.dll ../../../../../workingdir/bin",
			"{RMDIR} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp"
		}

	filter { "system:windows", "configurations:Release*" }
		defines { "PX_PHYSX_GPU_SHARED_LIB_NAME=PhysXGpu_64.dll" }

		postbuildcommands
		{
			"{MKDIR} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/release/temp",
			"{COPY} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/release/PhysXGpu_64.dll ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp",
			"{MOVE} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/release/temp/PhysXGpu_64.dll ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/debug/temp/PhysXGpu_64d.dll",
			"{COPY} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/release/temp/PhysXGpu_64d.dll ../../../../../workingdir/bin",
			"{RMDIR} ../../../../../src/Dependencies/PhysX/physx/bin/win.x86_64.vc141.mt/release/temp"
		}

	filter { "system:windows", "configurations:Profile*" }
		-- targetsuffix "64p"

	filter { "system:windows", "configurations:Optimized_Debug*" }
		-- targetsuffix "64od"

	filter { "system:not windows" }
		excludes
		{
			"physx/source/physx/src/device/windows/*.*",
			"physx/source/physx/src/windows/*.*" 
		}

		files { "physx/source/physx/src/device/linux/*.*" }

	filter {}

	includedirs
	{
		"physx/include/common",
		"physx/include/geometry",
		"physx/include/geomutils",
		"physx/include/gpu",
		"physx/include/pvd",
		"physx/source/common/include",
		"physx/source/common/src",
		"physx/source/geomutils/include",
		"physx/source/geomutils/src",
		"physx/source/geomutils/src/ccd",
		"physx/source/geomutils/src/common",
		"physx/source/geomutils/src/contact",
		"physx/source/geomutils/src/convex",
		"physx/source/geomutils/src/distance",
		"physx/source/geomutils/src/gjk",
		"physx/source/geomutils/src/hf",
		"physx/source/geomutils/src/intersection",
		"physx/source/geomutils/src/mesh",
		"physx/source/geomutils/src/pcm",
		"physx/source/geomutils/src/sweep",
		"physx/source/immediatemode/include",
		"physx/source/lowlevel/api/include",
		"physx/source/lowlevel/common/include/pipeline",
		"physx/source/lowlevel/software/include",
		"physx/source/lowlevelaabb/include",
		"physx/source/lowleveldynamics/include",
		"physx/source/physxcooking/src",
		"physx/source/physxcooking/src/convex",
		"physx/source/physxcooking/src/mesh",
		"physx/source/physxgpu/include",
		"physx/source/physxmetadata/core/include",
		"physx/source/physx/src/buffering",
		"physx/source/physx/src/device",
		"physx/source/physx/src",
		"physx/source/pvd/include",
		"physx/source/scenequery/include",
		"physx/source/simulationcontroller/include",
		"physx/source/simulationcontroller/src"
	}

	local physx_deps =
	{
		"LowLevel",
		"LowLevelAABB",
		"LowLevelDynamics",
		"PhysXCommon",
		"PhysXFoundation",
		"PhysXPvdSDK",
		"PhysXTask",
		"SceneQuery",
		"SimulationController"
	}

	dependson(physx_deps)
	links(physx_deps)


group "Dependencies"
