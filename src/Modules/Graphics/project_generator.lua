function DoGraphicsModule(renderer)
	project("Graphics" .. renderer)
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { "**.h", "**.cpp", "**.inl" }
		removefiles { "Shibboleth_GraphicsModule.cpp" }

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		flags { "FatalWarnings" }

		includedirs
		{
			"include",
			"../../Engine/Memory/include",
			"../../Engine/Engine/include",
			"../../Dependencies/EASTL/include",
			-- "../../Dependencies/rapidjson",
			-- "../../Dependencies/glm",
			-- "../../Dependencies/mpack",
			"../../Frameworks/Gaff/include",
			"../../Frameworks/Gleam/include"
		}


	project("Graphics" .. renderer .. "Module")
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { "Shibboleth_GraphicsModule.cpp" }

		ModuleGen("Graphics")
		ModuleCopy()

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
			links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		flags { "FatalWarnings" }

		filter { "system:windows" }
			links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

		filter {}

		includedirs
		{
			"include",
			"../../Engine/Memory/include",
			"../../Engine/Engine/include",
			"../../Dependencies/EASTL/include",
			-- -- "../../Dependencies/rapidjson",
			-- -- "../../Dependencies/glm",
			-- -- "../../Dependencies/mpack",
			"../../Frameworks/Gaff/include",
			"../../Frameworks/Gleam/include"
		}

		local deps =
		{
			"Graphics" .. renderer,
			"Memory",
			"Gaff",
			"Gleam",
			"Engine",
			"EASTL",
			"mpack"
			-- "Resource",
		}

		dependson(deps)
		links(deps)

		-- NewDeleteLinkFix()
end

DoGraphicsModule("Direct3D11")
-- DoGraphicsModule("Vulkan")
