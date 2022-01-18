This folder contains libraries that are slightly modified. I have also added a custom config header
for projects that require them.

While not that much different than the original source, these packages ARE NOT
the original source.

Libraries Affected:
	assimp
		Fixing RAPIDJSON_HAS_STDSTRING not being defined before inclusion of rapidjson.

	minizip
		Commented out a block that says #define _CRT_SECURE_NO_WARNINGS, as I have it defined at the project level.

	PhysX
		Changed #include <typeinfo.h> -> #include <typeinfo>.
		Changed GetModuleHandle() -> GetModuleHandleA().
		Fixed compilation error in GuGJKType.h not returning a const reference.

	imgui
		Fixed ImGui::DestroyContext() not checking for null.

	Catch2
		Wrapped main function definitions into a DO_NOT_GENERATE_MAIN block to optionally compile them out.
		Added my own main function that lives next to Catch2 code.

	Optick
		Made Optick::BeginFrame() and Optick::EndFrame() functions a little more friendly when frames overlap.

	zlib-ng
		Fixed compilation error on Linux when compiling on a system that does not support AVX.
