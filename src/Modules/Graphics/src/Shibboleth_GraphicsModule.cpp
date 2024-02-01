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

#include "Gen_ReflectionInit.h"
#include <Shibboleth_ModuleMacros.h>

SHIB_DEFINE_MODULE_BEGIN(Graphics)

#ifdef SHIB_STATIC
	#include <Log/Shibboleth_LogManager.h>

	namespace Graphics
	{
		class Module final : public Shibboleth::Module
		{
		public:
			bool preInit(Shibboleth::IApp& app) override;
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
			static constexpr const char8_t* const k_log_channel_name_graphics = u8"Graphics";
			static constexpr Gaff::Hash32 k_log_channel_graphics = Gaff::FNV1aHash32StringConst(k_log_channel_name_graphics);

			const Shibboleth::LogType msg_type = static_cast<Shibboleth::LogType>(type);
			Shibboleth::GetApp().getLogManager().logMessage(msg_type, k_log_channel_graphics, msg);
		}

		bool Module::preInit(Shibboleth::IApp& app)
		{
			// $TODO: Fix crash in shutdown due to static destruction order.
			static Shibboleth::ProxyAllocator g_graphics_allocator("Graphics");

			if (!Shibboleth::Module::preInit(app)) {
				return false;
			}

			Gleam::SetAllocator(&g_graphics_allocator);
			Gleam::SetLogFunc(GraphicsLog);

			return true;
		}
	}
#endif

SHIB_DEFINE_MODULE_END(Graphics)
