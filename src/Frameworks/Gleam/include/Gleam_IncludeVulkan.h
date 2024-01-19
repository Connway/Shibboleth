/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#pragma once

#ifdef PLATFORM_WINDOWS
	#define VULKAN_MODULE_NAME "vulkan-1.dll"
	#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(PLATFORM_LINUX)
	#define VULKAN_MODULE_NAME ".so"

	#ifdef VULKAN_USE_WAYLAND
	#else
		#define VK_USE_PLATFORM_XLIB_KHR

		#ifdef VULKAN_USE_XRANDR
			#define VK_USE_PLATFORM_XLIB_XRANDR_EXT
		#endif
	#endif
#elif defined(PLATFORM_MAC)
	#define VULKAN_MODULE_NAME ".dylib"
	#define VK_USE_PLATFORM_MACOS_MVK
#endif

#define VK_NO_PROTOTYPES
#include <vulkan.h>

#ifdef PLATFORM_WINDOWS
	#define VULKAN_SURFACE_EXT VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(PLATFORM_LINUX)
	#define VULKAN_SURFACE_EXT VK_KHR_XLIB_SURFACE_EXTENSION_NAME
	//#define VULKAN_SURFACE_EXT VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME
#elif defined(PLATFORM_MAC)
	#define VULKAN_SURFACE_EXT VK_MVK_MACOS_SURFACE_EXTENSION_NAME
#endif
