This folder contains libraries that are slightly modified. What I mean by this is
I added underscores to some of the function calls so that Visual Studio 2013 would
stop giving errors about using POSIX names instead of ISO C++ names and other similarly
trivial compilation fixes. I have also added a custom config header for projects
that require them. My config header simply includes the appropriate header for
the OS it is being compiled on.

LibRaw has tons of warnings and had some silly errors involving unary minus on
unsigned values. These have also been altered to compile.

While not that much different than the original source, these packages ARE NOT
the original source.

Libraries Affected:
	LibTIFF
	libjpeg
	zlib
