/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <Shibboleth_ISystem.h>
#include <EAThread/eathread_mutex.h>
#include <EASTL/functional.h>

NS_SHIBBOLETH

class ResourceManager final : public IManager
{
public:
	using ResourceStateCallback = eastl::function<void (const Vector<IResource*>&)>;

	ResourceManager(void);

	bool init(void) override;

	template <class T>
	Gaff::RefPtr<T> requestResourceT(HashStringTemp64<> name, bool delay_load = false)
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
		return requestResourceT<T>(HashStringTemp64<>(name, eastl::CharStrlen(name)), delay_load);
	}

	IResourcePtr requestResource(const char* name, bool delay_load = false)
	{
		return requestResource(HashStringTemp64<>(name, eastl::CharStrlen(name)), delay_load);
	}

	template <class T>
	Gaff::RefPtr<T> createResourceT(HashStringTemp64<> name)
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
		return createResourceT<T>(HashStringTemp64<>(name, eastl::CharStrlen(name)));
	}

	IResourcePtr createResource(const char* name, const Gaff::IReflectionDefinition& ref_def)
	{
		return createResource(HashStringTemp64<>(name, eastl::CharStrlen(name)), ref_def);
	}

	template <class T>
	Gaff::RefPtr<T> getResourceT(HashStringTemp64<> name)
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
		return getResourceT<T>(HashStringTemp64<>(name, eastl::CharStrlen(name)));
	}

	IResourcePtr getResource(const char* name)
	{
		return getResource(HashStringTemp64<>(name, eastl::CharStrlen(name)));
	}

	IResourcePtr createResource(HashStringTemp64<> name, const Gaff::IReflectionDefinition& ref_def);
	IResourcePtr requestResource(HashStringTemp64<> name, bool delay_load = false);
	IResourcePtr getResource(HashStringTemp64<> name);
	void waitForResource(const IResource& resource) const;

	const IFile* loadFileAndWait(const char* file_path);

	ResourceCallbackID registerCallback(const Vector<IResource*>& resources, const ResourceStateCallback& callback);
	void removeCallback(ResourceCallbackID id);

private:
	using FactoryFunc = void* (*)(Gaff::IAllocator&);

	struct CallbackData final
	{
		VectorMap<int32_t, ResourceStateCallback> callbacks{ ProxyAllocator("Resource") };
		Vector<IResource*> resources{ ProxyAllocator("Resource") };
		int32_t next_id = 0;
	};

	EA::Thread::Mutex _res_lock;
	Vector<IResource*> _resources{ ProxyAllocator("Resource") };

	EA::Thread::Mutex _removal_lock;
	Vector<const IResource*> _pending_removals{ ProxyAllocator("Resource") };

	VectorMap<Gaff::Hash32, FactoryFunc> _resource_factories{ ProxyAllocator("Resource") };

	Vector<Gaff::Hash64> _callback_keys{ ProxyAllocator("Resource") };
	VectorMap<Gaff::Hash64, CallbackData> _callbacks{ ProxyAllocator("Resource") };
	EA::Thread::Mutex _callback_lock;

	ProxyAllocator _allocator = ProxyAllocator("Resource");

	void checkAndRemoveResources(void);
	void checkCallbacks(void);

	void removeResource(const IResource& resource);
	void requestLoad(IResource& resource);

	friend class ResourceSystem;
	friend class IResource;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceManager);
};

class ResourceSystem : public ISystem
{
public:
	bool init(void) override;
	void update() override;

private:
	ResourceManager* _res_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceSystem);
};


template <class T>
static bool LoadRefPtr(const Gaff::ISerializeReader& reader, Gaff::RefPtr<T>& out)
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
static void SaveRefPtr(Gaff::ISerializeWriter& writer, const Gaff::RefPtr<T>& value)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");
	writer.writeString(value->getFilePath().getBuffer());
}

NS_END

SHIB_REFLECTION_DECLARE(ResourceManager)
SHIB_REFLECTION_DECLARE(ResourceSystem)


// Gaff::RefPtr
SHIB_TEMPLATE_REFLECTION_DECLARE(Gaff::RefPtr, T);

SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Gaff::RefPtr, T)
	.serialize(LoadRefPtr<T>, SaveRefPtr<T>)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Gaff::RefPtr, T)
