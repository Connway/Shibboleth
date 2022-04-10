filter { "configurations:Debug", "platforms:x86" }
	defines { "TARGET_SUFFIX_U8=\"32d\"", "TARGET_SUFFIX=\"32d\"" }
	targetsuffix "32d"

filter { "configurations:Release", "platforms:x86" }
	defines { "TARGET_SUFFIX_U8=\"32\"", "TARGET_SUFFIX=\"32\"" }
	targetsuffix "32"

filter { "configurations:Profile", "platforms:x86" }
	defines { "TARGET_SUFFIX_U8=\"32p\"", "TARGET_SUFFIX=\"32p\"" }
	targetsuffix "32p"

filter { "configurations:Optimized_Debug", "platforms:x86" }
	defines { "TARGET_SUFFIX_U8=\"32od\"", "TARGET_SUFFIX=\"32od\"" }
	targetsuffix "32od"

filter { "configurations:Static_Debug*", "platforms:x86" }
	defines { "TARGET_SUFFIX_U8=\"32sd\"", "TARGET_SUFFIX=\"32sd\"" }
	targetsuffix "32sd"

filter { "configurations:Static_Release*", "platforms:x86" }
	defines { "TARGET_SUFFIX_U8=\"32s\"", "TARGET_SUFFIX=\"32s\"" }
	targetsuffix "32s"

filter { "configurations:Debug", "platforms:x64" }
	defines { "TARGET_SUFFIX_U8=\"64d\"", "TARGET_SUFFIX=\"64d\"" }
	targetsuffix "64d"

filter { "configurations:Release", "platforms:x64" }
	defines { "TARGET_SUFFIX_U8=\"64\"", "TARGET_SUFFIX=\"64\"" }
	targetsuffix "64"

filter { "configurations:Profile", "platforms:x64" }
	defines { "TARGET_SUFFIX_U8=\"64p\"", "TARGET_SUFFIX=\"64p\"" }
	targetsuffix "64p"

filter { "configurations:Optimized_Debug", "platforms:x64" }
	defines { "TARGET_SUFFIX_U8=\"64od\"", "TARGET_SUFFIX=\"64od\"" }
	targetsuffix "64od"

filter { "configurations:Static_Debug*", "platforms:x64" }
	defines { "TARGET_SUFFIX_U8=\"64sd\"", "TARGET_SUFFIX=\"64sd\"" }
	targetsuffix "64sd"

filter { "configurations:Static_Release*", "platforms:x64" }
	defines { "TARGET_SUFFIX_U8=\"64s\"", "TARGET_SUFFIX=\"64s\"" }
	targetsuffix "64s"

filter {}
