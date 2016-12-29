This folder contains libraries that are slightly modified. What I mean by this is
I added underscores to some of the function calls so that Visual Studio 2015 would
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
	assimp (Changed '#include "../contrib/unzip/unzip.h' lines to "#include <unzip.h>", as I already have minizip compiled into a separate lib. Fixed compilation error because a variable wasn't initialized.)
	minizip (commented out a block that says #define _CRT_SECURE_NO_WARNINGS, as I have it defined at the project level)
	ResIL
	OtterUI (got rid of a lot of warnings in Visual Studio and fixed a couple inconsistencies and crash bugs)
	dirent (got rid of warnings in Visual Studio)
	EASTL (eabase.h line 358. Defining EA_COMPILER_HAS_C99_FORMAT_MACROS.)

Dependencies versions:
Assimp - 3.2
Brofiler - 1.1.1
bullet - 2.83.7
ChaiScript - 5.8.5
dirent - 1.21
EASTL - commit 51718cd804bd3a5b446a305826e164cf35776518
enet - 1.3.13
glew - 1.13.0
jemalloc - 4.2.1
libjpeg - 9b
libpng - 1.6.21
libtiff - 4.0.6
nana - 1.4.1
OtterUI - 1.3.6.3 commit a2c5243
RapidJSON - commit 07343d5e3361a940448bb5d2887bb5253a126f32
zlib - 1.2.8
