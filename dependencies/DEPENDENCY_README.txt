This folder contains libraries that are slightly modified. What I mean by this is
I added underscores to some of the function calls so that Visual Studio 2013 would
stop giving errors about using POSIX names instead of ISO C++ names and other similarly
trivial compilation fixes. I have also added a custom config header for projects
that require them. My config header simply includes the appropriate header for
the OS it is being compiled on.

While not that much different than the original source, these packages ARE NOT
the original source.

Libraries Affected:
	LibTIFF
	libjpeg
	zlib
	jansson (changed header files with the line #include <jansson_config.h> to #include "jansson_config.h" and moved the #include <windows.h> in hashtable_seed.c from line 98 to line 42 to remove compile warnings)
	assimp (Changed '#include "../contrib/unzip/unzip.h' lines to "#include <unzip.h>", as I already have minizip compiled into a separate lib. Fixed compilation error because a variable wasn't initialized.)
	minizip (commented out a block that says #define _CRT_SECURE_NO_WARNINGS, as I have it defined at the project level)
	ResIL
	LuaState (added const char* version of doFile and doString)
	OtterUI (got rid of a lot of warnings in Visual Studio and fixed a couple inconsistencies and crash bugs)
	dirent (got rid of warnings in Visual Studio)
	Boxer (added support for Abort/Retry/Ignore and Cancel/Try/Continue button configurations)


Dependencies versions:
Assimp - 3.2
Boxer - commit 339c30d
Brofiler - 1.1.1
bullet - 2.83.7
dirent - 1.21
enet - 1.3.13
glew - 1.13.0
jansson - 2.7
libjpeg - 9b
libpng - 1.6.21
libtiff - 4.0.6
LuaBridge - 2.0
LuaJIT - 2.0.4
LuaState - 2.0
OtterUI - 1.3.6.3 commit a2c5243
utf8-cpp - 2.3.4
zlib - 1.2.8
