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
	assimp (changed '#include "../contrib/unzip/unzip.h' lines to "#include <unzip.h>", as I already have minizip compiled into a separate lib)
	minizip (commented out a block that says #define _CRT_SECURE_NO_WARNINGS, as I have it defined at the project level)
	ResIL (il_header.cpp, moved #include "il_internal.h" above #ifndef IL_NO_CHEAD)
	OtterUI (System.cpp line 44, changed from "mGraphics = NULL;" to "mGraphics = OTTER_NEW(Graphics, ());")
