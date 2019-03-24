function DoGraphicsModule(renderer)
	local project_name = "Graphics" .. renderer

	project(project_name)
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { "**.h", "**.cpp", "**.inl" }
		defines { "SHIB_STATIC" }

		ModuleGen("Graphics")

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


	project(project_name .. "Module")
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { "Shibboleth_GraphicsModule.cpp" }

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

		ModuleIncludesAndLinks(project_name)

		local deps =
		{
			"Gleam",
			-- "Resource",
		}

		dependson(deps)
		links(deps)
end

DoGraphicsModule("Direct3D11")
-- DoGraphicsModule("Vulkan")
