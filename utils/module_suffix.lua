filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Debug*", "platforms:x86" }
	targetsuffix "32d"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Release*", "platforms:x86" }
	targetsuffix "32"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Analyze*", "platforms:x86" }
	targetsuffix "32a"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Profile*", "platforms:x86" }
	targetsuffix "32p"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Optimized_Debug*", "platforms:x86" }
	targetsuffix "32od"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Debug*", "platforms:x64" }
	targetsuffix "64d"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Release*", "platforms:x64" }
	targetsuffix "64"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Analyze*", "platforms:x64" }
	targetsuffix "64a"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Profile*", "platforms:x64" }
	targetsuffix "64p"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Optimized_Debug*", "platforms:x64" }
	targetsuffix "64od"

filter {}
