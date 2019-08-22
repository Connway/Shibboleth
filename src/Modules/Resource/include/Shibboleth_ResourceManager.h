/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_IResource.h"
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_IManager.h>
#include <EAThread/eathread_mutex.h>

NS_SHIBBOLETH

class ResourceManager : public IManager
{
public:
	ResourceManager(void);

	void allModulesLoaded(void) override;

	template <class T>
	Gaff::RefPtr<T> requestResourceT(Gaff::HashStringTemp64 name, bool delay_load = false)
	{
		IResourcePtr old_ptr = requestResource(name, delay_load);

		if (old_ptr) {
			return Gaff::RefPtr<T>(static_cast<T*>(old_ptr.release()), false);
		} else {
			return Gaff::RefPtr<T>();
		}
	}

	template <class T>
	Gaff::RefPtr<T> requestResourceT(const char* name, bool delay_load = false)
	{
		return requestResourceT<T>(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)), delay_load);
	}

	IResourcePtr requestResource(const char* name, bool delay_load = false)
	{
		return requestResource(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)), delay_load);
	}

	template <class T>
	Gaff::RefPtr<T> createResourceT(Gaff::HashStringTemp64 name)
	{
		static_assert(T::Creatable, "Resource is not a creatable type.");
		IResourcePtr old_ptr = createResource(name, Reflection<T>::GetReflectionDefinition());

		if (old_ptr) {
			return Gaff::RefPtr<T>(static_cast<T*>(old_ptr.release()), false);
		}
		else {
			return Gaff::RefPtr<T>();
		}
	}

	template <class T>
	Gaff::RefPtr<T> createResourceT(const char* name)
	{
		return createResourceT<T>(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)));
	}

	IResourcePtr createResource(const char* name, const Gaff::IReflectionDefinition& ref_def)
	{
		return createResource(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)), ref_def);
	}

	template <class T>
	Gaff::RefPtr<T> getResourceT(Gaff::HashStringTemp64 name)
	{
		IResourcePtr old_ptr = getResource(name);

		if (old_ptr) {
			return Gaff::RefPtr<T>(static_cast<T*>(old_ptr.release()), false);
		} else {
			return Gaff::RefPtr<T>();
		}
	}

	template <class T>
	Gaff::RefPtr<T> getResourceT(const char* name)
	{
		return getResourceT<T>(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)));
	}

	IResourcePtr getResource(const char* name)
	{
		return getResource(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)));
	}

	IResourcePtr createResource(Gaff::HashStringTemp64 name, const Gaff::IReflectionDefinition& ref_def);
	IResourcePtr requestResource(Gaff::HashStringTemp64 name, bool delay_load = false);
	IResourcePtr getResource(Gaff::HashStringTemp64 name);
	void waitForResource(const IResource& resource) const;

	const IFile* loadFileAndWait(const char* file_path);

private:
	using FactoryFunc = void* (*)(Gaff::IAllocator&);

	EA::Thread::Mutex _res_lock;
	Vector<IResource*> _resources;
	VectorMap<Gaff::Hash32, FactoryFunc> _resource_factories;
	ProxyAllocator _allocator = ProxyAllocator("Resource");

	void removeResource(const IResource& resource);
	void requestLoad(IResource& resource);

	friend class IResource;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceManager);
};


template <class T>
bool LoadRefPtr(const Gaff::ISerializeReader& reader, Gaff::RefPtr<T>& out)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");

	if (!reader.isString()) {
		return false;
	}

	const char* const res_path = reader.readString();
	out = GetApp().getManagerTFast<ResourceManager>().requestResourceT<T>(res_path);
	reader.freeString(res_path);

	return out;
}

template <class T>
void SaveRefPtr(Gaff::ISerializeWriter& writer, const Gaff::RefPtr<T>& value)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");
	writer.writeString(value->getFilePath().getBuffer());
}

NS_END

SHIB_REFLECTION_DECLARE(ResourceManager)


// Gaff::RefPtr
SHIB_TEMPLATE_REFLECTION_DECLARE(Gaff::RefPtr, T);
SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(Gaff::RefPtr, T)

SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN(Gaff::RefPtr, T)
	.serialize(LoadRefPtr<T>, SaveRefPtr<T>)
SHIB_TEMPLATE_REFLECTION_BUILDER_END(Gaff::RefPtr, T)
