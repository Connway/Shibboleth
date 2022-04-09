filter { "configurations:*Debug*", "platforms:x86" }
	targetsuffix "32d"

filter { "configurations:*Release*", "platforms:x86" }
	targetsuffix "32"

filter { "configurations:*Profile*", "platforms:x86" }
	targetsuffix "32p"

filter { "configurations:*Optimized_Debug*", "platforms:x86" }
	targetsuffix "32od"

filter { "configurations:*Static_Debug*", "platforms:x86" }
	targetsuffix "32sd"

filter { "configurations:*Static_Release*", "platforms:x86" }
	targetsuffix "32s"

filter { "configurations:*Debug*", "platforms:x64" }
	targetsuffix "64d"

filter { "configurations:*Release*", "platforms:x64" }
	targetsuffix "64"

filter { "configurations:*Profile*", "platforms:x64" }
	targetsuffix "64p"

filter { "configurations:*Optimized_Debug*", "platforms:x64" }
	targetsuffix "64od"

filter { "configurations:*Static_Debug*", "platforms:x64" }
	targetsuffix "64sd"

filter { "configurations:*Static_Release*", "platforms:x64" }
	targetsuffix "64s"

filter {}
