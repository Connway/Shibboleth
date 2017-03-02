/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

NS_SHIBBOLETH

class AngelScriptComponent final : public Component
{
public:
	~AngelScriptComponent(void);

	void allComponentsLoaded(void) override;

	void addToWorld(void) override;
	void removeFromWorld(void) override;

	void setScript(const AngelScriptResourcePtr& script);
	const AngelScriptResourcePtr& getScript(void) const;

	int32_t getPropertyIndex(const char* name) const;
	int32_t getPropertyIndex(Gaff::Hash32 name) const;

	int32_t getFunctionIndex(const char* name) const;
	int32_t getFunctionIndex(Gaff::Hash32 name) const;

	template <class T>
	const T& getProperty(const char* name) const;

	template <class T>
	const T& getProperty(Gaff::Hash32 name) const;

	template <class T>
	const T& getProperty(int32_t index) const;

	template <class T>
	T& getProperty(const char* name);

	template <class T>
	T& getProperty(Gaff::Hash32 name);

	template <class T>
	T& getProperty(int32_t index);

	void prepareMethod(const char* name);
	void prepareMethod(Gaff::Hash32 name);
	void prepareMethod(int32_t index);

	void callMethod(void);

	template <class T>
	void setArg(int32_t index, T value);

	template <class T>
	T getReturnValue(void);

private:
	struct PropertyData
	{
		int type_id;
		asITypeInfo* type_info;
		void* property;
	};

	asIScriptContext* _context = nullptr;
	asIScriptObject* _object = nullptr;
	const asITypeInfo* _type_info = nullptr;

	VectorMap<Gaff::Hash32, PropertyData> _property_map;
	VectorMap<Gaff::Hash32, asIScriptFunction*> _method_map;

	AngelScriptResourcePtr _res;

	void onScriptLoaded(IResource* res);

	SHIB_REFLECTION_CLASS_DECLARE(AngelScriptComponent);
};

NS_END

#include "Shibboleth_AngelScriptComponent.inl"

SHIB_REFLECTION_DECLARE(AngelScriptComponent)
