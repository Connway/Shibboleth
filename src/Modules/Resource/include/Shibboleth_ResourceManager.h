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
#include <mutex>

NS_SHIBBOLETH

class ResourceManager : public IManager
{
public:
	ResourceManager(void);

	void allModulesLoaded(void) override;

	template <class T>
	Gaff::RefPtr<T> requestResourceT(const char* name)
	{
		Gaff::RefPtr<T> ptr(static_cast<T*>(requestResource(name).get()));
		return ptr;
	}

	template <size_t size>
	IResourcePtr requestResource(const char (&name)[size])
	{
		return requestResource(Gaff::HashStringTemp64(name, size - 1));
	}

	IResourcePtr requestResource(const char* name)
	{
		return requestResource(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)));
	}

	IResourcePtr requestResource(Gaff::HashStringTemp64 name);

	void waitForResource(const IResource& resource) const;

private:
	using FactoryFunc = void* (*)(Gaff::IAllocator&);

	std::mutex _res_lock;
	Vector<IResource*> _resources;
	VectorMap<Gaff::Hash32, FactoryFunc> _resource_factories;
	ProxyAllocator _allocator = ProxyAllocator("Resource");

	void removeResource(const IResource* resource);

	static void ResourceFileLoadJob(void* data);

	friend class IResource;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceManager);
};


template <class T>
void LoadRefPtr(const Gaff::ISerializeReader& reader, Gaff::RefPtr<T>& out)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");
	const char* const res_path = reader.readString();
	out = GetApp().getManagerTFast<ResourceManager>().requestResourceT<T>(res_path);
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
