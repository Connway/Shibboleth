function PhysXStaticLib(proj_name)
	project(proj_name)

	if _ACTION then
		location(GetDependenciesLocation())
	end

	local wildcard = "PhysX_3.4/Source/" .. proj_name

	kind("StaticLib")
	language "C++"

	defines
	{
		"PX_PHYSX_STATIC_LIB",
		"PX_DEBUG=1",
		"PX_CHECKED=1",
		-- "PX_NVTX=1",
		"PX_SUPPORT_PVD=1",
		"PX_PHYSX_DLL_NAME_POSTFIX=DEBUG"
	}

	includedirs { "include" }

	files
	{
		wildcard .. "/**.h",
		wildcard .. "/**.cpp"
	}

	filter { "action:vs*" }
		defines
		{
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_WINSOCK_DEPRECATED_NO_WARNINGS"
		}

	filter {}

	-- if lib_type == "SharedLib" then
	-- 	postbuildcommands
	-- 	{
	-- 		"{MKDIR} ../../../../../workingdir/bin",
	-- 		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	-- 	}
	-- end
end

function PhysXSharedLib(proj_name)
end

group "Dependencies/PhysX"

PhysXStaticLib("LowLevel")
