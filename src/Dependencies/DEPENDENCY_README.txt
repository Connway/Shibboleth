This folder contains libraries that are slightly modified. I have also added a custom config header
for projects that require them.

While not that much different than the original source, these packages ARE NOT
the original source.

Libraries Affected:
	assimp
		Fixing RAPIDJSON_HAS_STDSTRING not being defined before inclusion of rapidjson.

	minizip-ng
		unzip.h and zip.h added #include <zlib.h>
		Fixing compilation error with CLOCK_MONOTONIC not being defined.

	PhysX
		Changed #include <typeinfo.h> -> #include <typeinfo>.
		Changed GetModuleHandle() -> GetModuleHandleA().
		Fixed compilation error in GuGJKType.h not returning a const reference.

	imgui
		Fixed ImGui::DestroyContext() not checking for null.

	Catch2
		Added my own main function that lives next to Catch2 code.

	zlib-ng
		Fixed compilation error on Linux when compiling on a system that does not support AVX.

	EASTL
		Fixed compilation error on Linux with bitvector.
		Fixed compilation error on Linux with deque.
		Fixed a compilation warning with an unreferenced variable in Strlcpy.
		Fixed compilation error with char8_t when using C++20 standard.
		Fixed compilation error with GCC when using C++20 standard with destructible_test_function.
		"Fixed"" compilation error with GCC in eastl::swap about maybe uninitialized variables. "Fix" is to disable the warning for now.

	premake-qt
		Fixed unnecessarily adding links libs to StaticLib projects.
		Added if QT_DIR or QTDIR environment variables are present, use $(QT_DIR) or $(QTDIR) as paths instead.

	tracy
		Fixing compilation errors with macros in Tracy.hpp incorrectly constexpr'ing a variable into a non-constexpr constructor.
		Fixing compilation errors on Linux by adding #include <cpuid.h>.

	argparse
		Commented out unused variables to fix compilation issues.

	GLFW
		Fixing compilation error with CLOCK_REALTIME and CLOCK_MONOTONIC not being defined.

	LuaJIT
		Modified msvcbuild.bat to use /MDd, /MD, /Od, and /Fd flags.
		Added lua_isinteger.
		Changed lua_getfield to match the Lua 5.4 function definition.

Libraries included, but not used:
	acl
		Technically nothing uses this, but in the future we will use this for animations.

	Capstone
		This is only included for building TracyServer. It is not used by the engine itself.

	sqlite
		Technically nothing uses this, but in the future might use this.

	zstd
		This is only included for building TracyServer and unused APIs in minizip-ng. It is not used by the engine itself.


Linux Library Dependencies:
In order to get code to compile on Linux, these libraries will need to be installed from your package manager.
At this time, the codebase doesn't fully build on Linux. Even if it did, until Vulkan support is added,
the codebase won't run.

	* libx11-dev
	* libxrandr-dev
	* libxi-dev
	* libgl1-mesa-dev (will be removed when Vulkan is implemented)
	* libglx-dev (will be removed when Vulkan is implemented)
	* libxcursor-dev (for GLFW)
	* libxinerama-dev (for GLFW)
	* libgtk-3-dev (for TracyProfiler)
