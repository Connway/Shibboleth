filter { "configurations:Debug*", "platforms:x86" }
	targetsuffix "32d"

filter { "configurations:Release*", "platforms:x86" }
	targetsuffix "32"

filter { "configurations:Analyze*", "platforms:x86" }
	targetsuffix "32a"

filter { "configurations:Profile*", "platforms:x86" }
	targetsuffix "32p"

filter { "configurations:Optimized_Debug*", "platforms:x86" }
	targetsuffix "32od"

filter { "configurations:Debug*", "platforms:x64" }
	targetsuffix "64d"

filter { "configurations:Release*", "platforms:x64" }
	targetsuffix "64"

filter { "configurations:Analyze*", "platforms:x64" }
	targetsuffix "64a"

filter { "configurations:Profile*", "platforms:x64" }
	targetsuffix "64p"

filter { "configurations:Optimized_Debug*", "platforms:x64" }
	targetsuffix "64od"

filter {}
