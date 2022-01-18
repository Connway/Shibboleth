project "optick"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "SharedLib"
	language "C++"
	-- warnings "Default"

	files { "**.c", "**.h", "**.cpp" }

	-- Until we get proper Vulkan support, we'll disable it for now.
	defines { "OPTICK_EXPORTS", "OPTICK_ENABLE_GPU_VULKAN=0" }

	-- filter { "system:windows" }
	-- 	includedirs { "$(VULKAN_SDK)/Include" }

	-- filter {}

	-- libdirs
	-- {
	-- 	"$(VULKAN_SDK)/Lib",
	-- }

	--[[links
	{
		"vulkan-1"
	}--]]

	filter { "system:windows" }
		links
		{
			"d3d12",
			"dxgi",
			-- "vulkan-1"
		}

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	SetupConfigMap()
