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

#include "Shibboleth_RuntimeVarManager.h"
#include "Reflection/Shibboleth_ReflectionBase.h"
#include "Shibboleth_Utilities.h"

NS_SHIBBOLETH

class IModule
{
public:
	virtual ~IModule(void) {}

	virtual void initReflectionEnums(void) = 0;
	virtual void initReflectionAttributes(void) = 0;
	virtual void initReflectionClasses(void) = 0;

	virtual bool preInit(IApp& app)
	{
		SetApp(app);

	#ifdef SHIB_RUNTIME_VAR_ENABLED
		RegisterRuntimeVars();
	#endif

		return true;
	}

	virtual bool postInit(void) { return true; }

	virtual void shutdown(void) {}

protected:
	// This function is marked virtual so that it runs inside the context
	// of the loaded DLL and not inside Game_App.
	// It is not intended for the user to actually override it.
	virtual void initNonOwned(void)
	{
		// Initialize Enums.
		Refl::InitEnumReflection();

		// Initialize Attributes.
		Refl::InitAttributeReflection();

		// Initialize Classes.
		Refl::InitClassReflection();
	}

	virtual void initNonOwnedEnums(void)
	{
		Refl::InitEnumReflection();
	}

	virtual void initNonOwnedAttributes(void)
	{
		Refl::InitAttributeReflection();
	}

	virtual void initNonOwnedClasses(void)
	{
		Refl::InitClassReflection();
	}


	friend class App;
};

NS_END
