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
#include <Shibboleth_AngelScriptManager.h>
#include <Shibboleth_ModuleMacros.h>

SHIB_DEFINE_MODULE_BEGIN(Logic)

#ifdef SHIB_STATIC
	namespace Logic
	{
		class Module final : public Shibboleth::Module
		{
		public:
			bool preInit(Shibboleth::IApp& app) override;
			bool postInit() override;
		};

		bool Module::preInit(Shibboleth::IApp& app)
		{
			static Shibboleth::ProxyAllocator g_logic_allocator("Logic");

			if (!Shibboleth::Module::preInit(app)) {
				return false;
			}

			app.getReflectionManager().registerTypeBucket(CLASS_HASH(Esprit::ICondition));
			app.getReflectionManager().registerTypeBucket(CLASS_HASH(Esprit::IProcess));

			Esprit::SetAllocator(&g_logic_allocator);

			return true;
		}

		bool Module::postInit()
		{
			Shibboleth::AngelScriptManager::InitModuleThread();
			return true;
		}

	}
#endif

SHIB_DEFINE_MODULE_END(Logic)
