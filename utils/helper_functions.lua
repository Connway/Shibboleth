function ModuleGen(module_name)
	prebuildmessage("Generating Gen_ReflectionInit.h for module " .. module_name .. "!")
	prebuildcommands
	{
		"py ../../../utils/gen_module_file.py ../../../src/Modules/" .. module_name
	}
end

function ModuleCopy()
	postbuildcommands
	{
		"{MKDIR} ../../../workingdir/App/Modules",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../workingdir/App/Modules"
	}
end

function NewDeleteLinkFix()
	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

	filter { "system:windows", "configurations:Debug*", "platforms:x64" }
		links
		{
			"../../../build/" .. _ACTION .. "/output/x64/Debug/Shared.lib",
			"msvcrtd.lib"
		}

	filter { "system:windows", "configurations:Release*", "platforms:x64" }
		links
		{
			"../../../build/" .. _ACTION .. "/output/x64/Release/Shared.lib",
			"msvcrt.lib"
		}

	filter { "system:windows", "configurations:Analyze*", "platforms:x64" }
		links
		{
			"../../../build/" .. _ACTION .. "/output/x64/Release/Shared.lib",
			"msvcrt.lib"
		}

	filter { "system:windows", "configurations:Optimized_Debug*", "platforms:x64" }
		links
		{
			"../../../build/" .. _ACTION .. "/output/x64/Debug/Shared.lib",
			"msvcrtd.lib"
		}

	filter { "system:windows", "configurations:Profile*", "platforms:x64" }
		links
		{
			"../../../build/" .. _ACTION .. "/output/x64/Release/Shared.lib",
			"msvcrt.lib"
		}

	filter {}
end
