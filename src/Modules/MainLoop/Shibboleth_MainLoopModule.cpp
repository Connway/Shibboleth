/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

	#include <Shibboleth_Utilities.h>

	namespace MainLoop
	{

		bool Initialize(Shibboleth::IApp& app, Shibboleth::InitMode mode)
		{
			if (mode == Shibboleth::InitMode::EnumsAndFirstInits) {
				Shibboleth::SetApp(app);

			} else if (mode == Shibboleth::InitMode::Regular) {
				// Initialize Enums.
				Gaff::InitEnumReflection();

				// Initialize Attributes.
				Gaff::InitAttributeReflection();

				app.getReflectionManager().registerTypeBucket(CLASS_HASH(ISystem));
			}

			Gen::InitReflection(mode);

			return true;
		}

	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app, Shibboleth::InitMode mode)
	{
		return MainLoop::Initialize(app, mode);
	}

	DYNAMICEXPORT_C void InitModuleNonOwned(void)
	{
		MainLoop::InitializeNonOwned();
	}

	DYNAMICEXPORT_C bool SupportsHotReloading(void)
	{
		return false;
	}

#endif
