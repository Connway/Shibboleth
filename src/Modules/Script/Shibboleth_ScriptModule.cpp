/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

	#include "Shibboleth_ScriptConfigs.h"
	#include <Shibboleth_JobPool.h>
	#include <Shibboleth_IModule.h>
	#include <Gaff_JSON.h>

	namespace Script
	{
		class Module final : public Shibboleth::IModule
		{
		public:
			bool preInit(Shibboleth::IApp& app) override;
			void initReflectionEnums(void) override;
			void initReflectionAttributes(void) override;
			void initReflectionClasses(void) override;
		};

		bool Module::preInit(Shibboleth::IApp& app)
		{
			IModule::preInit(app);

			const Gaff::JSON script_threads = app.getConfigs().getObject(Shibboleth::k_config_script_threads);
			const int32_t num_threads = script_threads.getInt32(Shibboleth::k_config_script_default_num_threads);

			app.getJobPool().addPool(Shibboleth::HashStringView32<>(Shibboleth::k_config_script_thread_pool_name), num_threads);

			return true;
		}

		void Module::initReflectionEnums(void)
		{
			Gen::Script::InitReflection(InitMode::Enums);
		}

		void Module::initReflectionAttributes(void)
		{
			Gen::Script::InitReflection(InitMode::Attributes);
		}

		void Module::initReflectionClasses(void)
		{
			Gen::Script::InitReflection(InitMode::Classes);
		}

		Shibboleth::IModule* CreateModule(void)
		{
			return SHIB_ALLOCT(Script::Module, Shibboleth::ProxyAllocator("Script"));
		}
	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C Shibboleth::IModule* CreateModule(void)
	{
		return Script::CreateModule();
	}

#endif
