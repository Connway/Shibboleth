group "Modules/Graphics"

function DoGraphicsModule(renderer)
	project("GraphicsModule" .. renderer)
		if _ACTION then
			location("../../../project/" .. _ACTION .. "/graphics")
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
			"../../Memory/include",
			"../../Shared/include",
			"../../../dependencies/EASTL/include",
			"../../../dependencies/angelscript/angelscript/include",
			-- "../../../dependencies/rapidjson",
			-- "../../../dependencies/glm",
			-- "../../../dependencies/mpack",
			"../../../frameworks/Gaff/include",
			"../../../frameworks/Gleam/include",
			"../Entity/include",
			"../Scripting/include"
		}

		local dependencies =
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

		dependson(dependencies)
		links(dependencies)

		-- NewDeleteLinkFix()
end

DoGraphicsModule("Direct3D11")
-- DoGraphicsModule("Vulkan")
