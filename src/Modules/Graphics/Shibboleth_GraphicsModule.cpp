/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#ifndef SHIB_IS_BASE

#include "Gen_ReflectionInit.h"

#ifdef SHIB_STATIC

	#include <Shibboleth_LogManager.h>
	#include <Shibboleth_IModule.h>
	#include <Gleam_Global.h>

	namespace Graphics
	{
		class Module final : public Shibboleth::IModule
		{
		public:
			bool preInit(Shibboleth::IApp& app) override;
			void initReflectionEnums(void) override;
			void initReflectionAttributes(void) override;
			void initReflectionClasses(void) override;
		};

		//static void* GraphicsAlloc(size_t size)
		//{
		//	return SHIB_ALLOC(size, g_graphics_allocator);
		//}

		//static void* GraphicsCalloc(size_t num, size_t size)
		//{
		//	return SHIB_CALLOC(num, size, g_graphics_allocator);
		//}

		//static void* GraphicsRealloc(void* old_ptr, size_t new_size)
		//{
		//	return SHIB_REALLOC(old_ptr, new_size, g_graphics_allocator);
		//}

		static void GraphicsLog(const char8_t* msg, Gleam::LogMsgType type)
		{
			const Shibboleth::LogType msg_type = static_cast<Shibboleth::LogType>(type);
			Shibboleth::GetApp().getLogManager().logMessage(msg_type, Shibboleth::k_log_channel_default, msg);
		}

		bool Module::preInit(Shibboleth::IApp& app)
		{
			// $TODO: Fix crash in shutdown due to static destruction order.
			static Shibboleth::ProxyAllocator g_graphics_allocator("Graphics");

			IModule::preInit(app);

			Gleam::SetAllocator(&g_graphics_allocator);
			Gleam::SetLogFunc(GraphicsLog);

			return true;
		}

		void Module::initReflectionEnums(void)
		{
			// Should NOT add other code here.
			Gen::Graphics::InitReflection(InitMode::Enums);
		}

		void Module::initReflectionAttributes(void)
		{
			// Should NOT add other code here.
			Gen::Graphics::InitReflection(InitMode::Attributes);
		}

		void Module::initReflectionClasses(void)
		{
			// Should NOT add other code here.
			Gen::Graphics::InitReflection(InitMode::Classes);
		}

		Shibboleth::IModule* CreateModule(void)
		{
			return SHIB_ALLOCT(Graphics::Module, Shibboleth::ProxyAllocator("Graphics"));
		}
	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C Shibboleth::IModule* CreateModule(void)
	{
		return Graphics::CreateModule();
	}

#endif

#endif
