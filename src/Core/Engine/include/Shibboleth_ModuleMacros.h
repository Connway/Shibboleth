/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#ifdef SHIB_STATIC
	#include "Shibboleth_IModule.h"

	#define SHIB_DEFINE_MODULE_BEGIN(ModuleName) \
		NS_SHIBBOLETH \
			class Module : public Shibboleth::IModule \
			{ \
			public: \
				void initReflectionEnums(void) override; \
				void initReflectionAttributes(void) override; \
				void initReflectionClasses(void) override; \
			}; \
			void Module::initReflectionEnums(void) \
			{ \
				Gen::ModuleName::InitReflection(Gen::InitMode::Enums); \
			} \
			void Module::initReflectionAttributes(void) \
			{ \
				Gen::ModuleName::InitReflection(Gen::InitMode::Attributes); \
			} \
			void Module::initReflectionClasses(void) \
			{ \
				Gen::ModuleName::InitReflection(Gen::InitMode::Classes); \
			} \
		NS_END

	#define SHIB_DEFINE_MODULE_END(ModuleName) \
		namespace ModuleName \
		{ \
			Shibboleth::IModule* CreateModule(void) \
			{ \
				return SHIB_ALLOCT(ModuleName::Module, Shibboleth::ProxyAllocator(#ModuleName)); \
			} \
		}

	#define SHIB_DEFINE_BASIC_MODULE(ModuleName) \
		SHIB_DEFINE_MODULE_BEGIN(ModuleName) \
		namespace ModuleName \
		{ \
			class Module final : public Shibboleth::Module \
			{ \
			}; \
		} \
		SHIB_DEFINE_MODULE_END(ModuleName) \

#else
	#include <Gaff_Defines.h>

	#define SHIB_DEFINE_MODULE_BEGIN(ModuleName)
	#define SHIB_DEFINE_MODULE_END(ModuleName) \
		GAFF_DYNAMIC_EXPORT_C Shibboleth::IModule* CreateModule(void) \
		{ \
			return ModuleName::CreateModule(); \
		}

	#define SHIB_DEFINE_BASIC_MODULE(ModuleName) SHIB_DEFINE_MODULE_END(ModuleName)

#endif
