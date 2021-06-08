filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Debug*", "platforms:x86" }
	targetsuffix "32d"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Release*", "platforms:x86" }
	targetsuffix "32"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Profile*", "platforms:x86" }
	targetsuffix "32p"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Optimized_Debug*", "platforms:x86" }
	targetsuffix "32od"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Static_Debug*", "platforms:x86" }
	targetsuffix "32sd"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Static_Release*", "platforms:x86" }
	targetsuffix "32s"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Debug*", "platforms:x64" }
	targetsuffix "64d"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Release*", "platforms:x64" }
	targetsuffix "64"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Profile*", "platforms:x64" }
	targetsuffix "64p"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Optimized_Debug*", "platforms:x64" }
	targetsuffix "64od"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Static_Debug*", "platforms:x64" }
	targetsuffix "64sd"

filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:*Static_Release*", "platforms:x64" }
	targetsuffix "64s"

filter {}
