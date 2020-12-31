This folder contains libraries that are slightly modified. What I mean by this is
I added underscores to some of the function calls so that Visual Studio 2017 would
stop giving errors about using POSIX names instead of ISO C++ names and other similarly
trivial compilation fixes. I have also added a custom config header for projects
that require them. My config header simply includes the appropriate header for
the OS it is being compiled on.

While not that much different than the original source, these packages ARE NOT
the original source.

Libraries Affected:
	LibTIFF
		Can't remember, probably compilation errors.

	minizip
		Commented out a block that says #define _CRT_SECURE_NO_WARNINGS, as I have it defined at the project level

	zlib-ng
		Adding OF macro and STDC defines for compatability with minizip.

	PhysX
		Changed #include <typeinfo.h> -> #include <typeinfo>.
		Changed GetModuleHandle() -> GetModuleHandleA().

	imgui
		Fixed ImGui::DestroyContext() not checking for null.

	Catch2
		Wrapped main function definitions into a DO_NOT_GENERATE_MAIN block to optionally compile them out.
		Added my own main function that lives next to Catch2 code.
