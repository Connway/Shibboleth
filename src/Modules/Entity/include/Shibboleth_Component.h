/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include <Shibboleth_Reflection.h>
#include <Shibboleth_String.h>

NS_SHIBBOLETH

class Object;

class Component : public Gaff::IReflectionObject
{
public:
	template <class T>
	static T* Create(void)
	{
		const Vector<const Gaff::IReflectionDefinition*>* const components = GetApp().getReflectionManager().getTypeBucket(Gaff::FNV1aHash64Const("Component"));
		GAFF_ASSERT(components);

		const auto it = eastl::lower_bound(components->begin(), components->end(), component_name, ReflectionManager::CompareRefHash);
		GAFF_ASSERT(it != components->end() && (*it)->getReflectionInstance().getHash() == component_name);

		ProxyAllocator allocator("Components");
		return (*it)->createAllocT<T>(allocator);
	}

	static void* Create(Gaff::Hash64 component_name)
	{
		const Vector<const Gaff::IReflectionDefinition*>* const components = GetApp().getReflectionManager().getTypeBucket(Gaff::FNV1aHash64Const("Component"));
		GAFF_ASSERT(components);

		const auto it = eastl::lower_bound(components->begin(), components->end(), component_name, ReflectionManager::CompareRefHash);
		GAFF_ASSERT(it != components->end() && (*it)->getReflectionInstance().getHash() == component_name);

		ProxyAllocator allocator("Components");
		return (*it)->createAlloc(allocator);
	}

	Component(void);
	virtual ~Component(void);

	//virtual const Gaff::JSON& getSchema(void) const;
	//virtual bool validate(const Gaff::JSON& json);

	virtual void load(const Gaff::ISerializeReader& reader);
	virtual bool save(Gaff::ISerializeWriter& writer);

	virtual void onAllComponentsLoaded(void);

	virtual void onAddToWorld(void);
	virtual void onRemoveFromWorld(void);

	virtual void setActive(bool active);
	virtual bool isActive(void) const;

	const U8String& getName(void) const;
	void setName(const char* name);

	const Object* getOwner(void) const;
	Object* getOwner(void);
	void setOwner(Object* owner);

	size_t getIndex(void) const;
	void setIndex(size_t index);

private:
	U8String _name;
	Object* _owner;
	size_t _comp_index;
	bool _active;

	GAFF_NO_COPY(Component);
	GAFF_NO_MOVE(Component);
};

NS_END
