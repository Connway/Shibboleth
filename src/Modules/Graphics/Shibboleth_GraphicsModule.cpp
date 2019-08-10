/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Gen_ReflectionInit.h"

#ifdef SHIB_STATIC

	#include <Shibboleth_ProxyAllocator.h>
	#include <Shibboleth_Utilities.h>
	#include <Gleam_RenderDevice.h>
	#include <Gleam_Window.h>
	#include <Gleam_Global.h>

	namespace Graphics
	{

		static Shibboleth::ProxyAllocator g_graphics_allocator("Graphics");

		//static void* ImageAlloc(size_t size)
		//{
		//	return SHIB_ALLOC(size, g_graphics_allocator);
		//}

		//static void* ImageCalloc(size_t num, size_t size)
		//{
		//	return Shibboleth::ShibbolethCalloc(num, size, g_graphics_allocator.getPoolIndex());
		//}

		//static void* ImageRealloc(void* old_ptr, size_t new_size)
		//{
		//	return Shibboleth::ShibbolethRealloc(old_ptr, new_size, g_graphics_allocator.getPoolIndex());
		//}

		bool Initialize(Shibboleth::IApp& app)
		{
			Shibboleth::SetApp(app);
			Gen::InitReflection();

			Gleam::SetAllocator(&g_graphics_allocator);

			return true;
		}

		void Shutdown(void)
		{
			Gleam::RenderDevice::Cleanup();
			Gleam::Window::Cleanup();
		}
	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
	{
		return Graphics::Initialize(app);
	}

	DYNAMICEXPORT_C void ShutdownModule()
	{
		Graphics::Shutdown();
	}

#endif