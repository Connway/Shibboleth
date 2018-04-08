function DoGraphicsModule(renderer)
	project("GraphicsModule" .. renderer)
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		-- files { "Shibboleth_GraphicsModule.cpp" }
		files { "**.h", "**.cpp", "**.inl" }

		ModuleGen("Graphics")
		ModuleCopy()

		if renderer == "Direct3D11" then
			defines { "USE_D3D11" }
			links { "d3d11", "D3dcompiler", "dxgi", "dxguid" }
		elseif renderer == "Vulkan" then
			defines { "USE_VULKAN" }
		end

		filter { "configurations:not Analyze*" }
			flags { "FatalWarnings" }

		filter { "system:windows" }
			links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

		filter {}

		includedirs
		{
			"include",
			"../../Engine/Memory/include",
			"../../Engine/Shared/include",
			"../../Dependencies/EASTL/include",
			"../../Dependencies/angelscript/angelscript/include",
			-- "../../Dependencies/rapidjson",
			-- "../../Dependencies/glm",
			-- "../../Dependencies/mpack",
			"../../Frameworks/Gaff/include",
			"../../Frameworks/Gleam/include",
			"../Entity/include",
			"../Scripting/include"
		}

		local deps =
		{
			"Memory",
			"Gaff",
			"Gleam",
			"Shared",
			"EASTL",
			"Entity",
			"Scripting",
			"Resource",
			"angelscript",
			-- "mpack"
		}

		dependson(deps)
		links(deps)

		-- NewDeleteLinkFix()
end

DoGraphicsModule("Direct3D11")
-- DoGraphicsModule("Vulkan")
