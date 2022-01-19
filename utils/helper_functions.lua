function ModuleDependencies(module_name)
	return {
		"Memory",
		"Engine",
		"EASTL",
		"Gaff",
		"mpack",
		module_name
	}
end

function ModuleIncludesAndLinks(module_name, base_name)
	local base_dir = ""

	if base_name then
		base_dir = GetModulesDirectory(base_name)
	else
		base_dir = GetModulesDirectory(module_name)
	end

	includedirs
	{
		base_dir .. "include",
		base_dir .. "../../Frameworks/Gaff/include",
	}

	local deps = ModuleDependencies(module_name)
	dependson(deps)
	links(deps)
end

function StaticHeaderGen()
	prebuildmessage("Generating Gen_ReflectionInit.h for static build!")

	filter { "system:windows" }
		prebuildcommands
		{
			"cd ../../../../../utils",
			"premake5 gen_static_header"
		}

	filter { "system:not windows" }
		prebuildcommands
		{
			"cd ../../../../../utils && ./premake5 gen_static_header"
		}

	filter {}
end

function StaticLinks()
	local filter_deps = {}

	local ProcessModule = function(dir)
		if not os.isfile(dir .. "/project_generator.lua") then
			return
		end

		local funcs = dofile(dir .. "/project_generator.lua")
		funcs.LinkDependencies()

		if funcs.FilterDependencies then
			table.insert(filter_deps, funcs.FilterDependencies)
		end
	end

	local module_generators = os.matchdirs("../../Modules/*")
	table.foreachi(module_generators, ProcessModule)

	table.foreachi(filter_deps, function(func) func() end)

	-- filter { "configurations:Static_*_D3D11" }
	-- 	dependson("GraphicsDirect3D11")
	-- 	links("GraphicsDirect3D11")

	-- -- filter { "configurations:Static_*_Vulkan" }
	-- -- 	dependson("GraphicsVulkan")
	-- -- 	links("GraphicsVulkan")
end

function ModuleGen(module_name)
	prebuildmessage("Generating Gen_ReflectionInit.h for module " .. module_name .. "!")
	prebuildcommands
	{
		"cd ../../../../../utils",
		"premake5 gen_module_header --module=" .. module_name
	}
end

function ModuleCopy(dir)
	if dir == nil then
		dir = "Modules"
	end

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/" .. dir,
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/" .. dir
	}
end

function NewDeleteLinkFix()
	if _ACTION == nil then
		return
	end

	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

	filter { "system:windows", "configurations:Debug*", "platforms:x64" }
		links
		{
			"../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Debug/Engine.lib",
			"msvcrtd.lib"
		}

	filter { "system:windows", "configurations:Release*", "platforms:x64" }
		links
		{
			"../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Release/Engine.lib",
			"msvcrt.lib"
		}

	filter { "system:windows", "configurations:Optimized_Debug*", "platforms:x64" }
		links
		{
			"../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Debug/Engine.lib",
			"msvcrtd.lib"
		}

	filter { "system:windows", "configurations:Profile*", "platforms:x64" }
		links
		{
			"../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Release/Engine.lib",
			"msvcrt.lib"
		}

	filter {}
end

function GetActionLocation()
	if _ACTION then
		return "../../../.generated/project/" .. os.target() .. "/" .. _ACTION
	else
		return ""
	end
end

function GetDependenciesLocation()
	return GetActionLocation() .. "/dependencies"
end

function GetFrameworkLocation()
	return GetActionLocation() .. "/frameworks"
end

function GetModulesLocation()
	return "../.generated/project/" .. os.target() .. "/" .. _ACTION .. "/modules"
end

function GetEngineLocation()
	return GetActionLocation() .. "/engine"
end

function GetToolsLocation()
	return GetActionLocation() .. "/tools"
end

function GetTestsLocation()
	if _ACTION then
		return "../../.generated/project/" .. os.target() .. "/" .. _ACTION .. "/tests"
	else
		return ""
	end
end

function GetModulesDirectory(module_name)
	return "../src/Modules/" .. module_name .. "/"
end

function RunFile(file)
	dofile(file)
end

function IncludeWxWidgets(is_not_module)
	local base_dir = "../src/"

	if is_not_module then
		base_dir = "../../"
	end

	includedirs { base_dir .. "Dependencies/wxWidgets/include" }
	defines
	{
		"NOPCH",
		-- "WXBUILDING",
		"wxUSE_ZLIB_H_IN_PATH",
		-- "wxUSE_GUI=1",
		-- "wxUSE_BASE=1",
		"WXUSINGDLL"
	}

	filter { "system:windows" }
		includedirs { base_dir .. "Dependencies/wxWidgets/include/msvc" }
		defines { "__WXMSW__" }

	filter {}
end

function SetupConfigMap()
	configmap {
		["Static_Debug_D3D11"] = "Debug",
		["Static_Release_D3D11"] = "Release"
	}
end
