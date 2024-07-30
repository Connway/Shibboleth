This folder contains libraries that are slightly modified. I have also added a custom config header
for projects that require them.

Dependencies with modifications will be tagged with $MODIFICATION in the source code.
Dependencies that have been modified, but not updated to have the tag are listed below.

While not that much different than the original source, these packages ARE NOT
the original source.

Libraries Affected:
	imgui
		Fixed ImGui::DestroyContext() not checking for null.

	premake-qt
		Added if QT_DIR or QTDIR environment variables are present, use $(QT_DIR) or $(QTDIR) as paths instead.

	tracy
		Fixing compilation errors with macros in Tracy.hpp incorrectly constexpr'ing a variable into a non-constexpr constructor.
		Fixing compilation errors on Linux by adding #include <cpuid.h>.
		Fixing unused variable warnings on Linux/Mac in TracyLua.hpp.

	argparse
		Commented out unused variables to fix compilation issues.

Libraries included, but not used:
	acl
		Technically nothing uses this, but in the future we will use this for animations.

	Capstone
		This is only included for building TracyServer. It is not used by the engine itself.

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
