This folder contains libraries that are slightly modified. What I mean by this is
I added underscores to some of the function calls so that Visual Studio 2015/2017 would
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
	dirent (got rid of warnings in Visual Studio)
	libuv (fixing potentially uninitialized pointer errors)
	zlib-ng (fixing compilation errors, adding OF macro, adding STDC defines)
	wxWidgets (Fixing setup.h wxSETUPH_PATH macro. Fixing compiler errors with VS2017.)