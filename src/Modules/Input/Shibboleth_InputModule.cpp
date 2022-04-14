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

	#include <Shibboleth_IModule.h>

	namespace Input
	{
		class Module final : public Shibboleth::IModule
		{
		public:
			void initReflectionEnums(void) override;
			void initReflectionAttributes(void) override;
			void initReflectionClasses(void) override;
		};

		void Module::initReflectionEnums(void)
		{
			// Should NOT add other code here.
			Gen::Input::InitReflection(InitMode::Enums);
		}

		void Module::initReflectionAttributes(void)
		{
			// Should NOT add other code here.
			Gen::Input::InitReflection(InitMode::Attributes);
		}

		void Module::initReflectionClasses(void)
		{
			// Should NOT add other code here.
			Gen::Input::InitReflection(InitMode::Classes);
		}

		Shibboleth::IModule* CreateModule(void)
		{
			return SHIB_ALLOCT(Input::Module, Shibboleth::ProxyAllocator("Input"));
		}
	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C Shibboleth::IModule* CreateModule(void)
	{
		return Input::CreateModule();
	}

#endif
