filter { "configurations:Debug*", "platforms:x86" }
	targetsuffix "32d"

filter { "configurations:Release*", "platforms:x86" }
	targetsuffix "32"

filter { "configurations:Debug*", "platforms:x64" }
	targetsuffix "64d"

filter { "configurations:Release*", "platforms:x64" }
	targetsuffix "64"

filter {}