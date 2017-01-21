/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include <Shibboleth_AngelScriptResource.h>
#include <Shibboleth_Component.h>

class asIScriptContext;
class asIScriptObject;
class asITypeInfo;

NS_SHIBBOLETH

class AngelScriptComponent final : public Component
{
public:
	~AngelScriptComponent(void);

	void allComponentsLoaded(void) override;

	void addToWorld(void) override;
	void removeFromWorld(void) override;

private:
	asIScriptContext* _context = nullptr;
	asIScriptObject* _object = nullptr;
	const asITypeInfo* _type_info = nullptr;
	AngelScriptResourcePtr _res;

	SHIB_REFLECTION_CLASS_DECLARE(AngelScriptComponent);
};

NS_END

SHIB_REFLECTION_DECLARE(AngelScriptComponent)
