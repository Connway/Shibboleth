function ModuleGen(module_name)
	prebuildmessage("Generating Gen_ReflectionInit.h for module " .. module_name .. "!")
	prebuildcommands
	{
		"py ../../../utils/gen_module_file.py ../../../src/" .. module_name
	}
end

function ModuleCopy()
	postbuildcommands
	{
		"{MKDIR} ../../../workingdir/App/Modules",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../workingdir/App/Modules"
	}
end
