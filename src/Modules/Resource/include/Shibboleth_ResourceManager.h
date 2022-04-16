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

#pragma once

#include "Shibboleth_IResource.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_AppUtils.h>
#include <eathread/eathread_mutex.h>
#include <EASTL/functional.h>

NS_SHIBBOLETH

class ResourceManager final : public IManager
{
public:
	using ResourceStateCallback = eastl::function<void (const Vector<IResource*>&)>;

	ResourceManager(void);
	~ResourceManager(void);

	bool init(void) override;

	template <class T>
	Gaff::RefPtr<T> requestResourceT(HashStringView64<> name, bool delay_load = false)
	{
		IResourcePtr old_ptr = requestResource(name, delay_load);

		if (old_ptr) {
			return Gaff::RefPtr<T>(static_cast<T*>(old_ptr.release()), false);
		} else {
			return Gaff::RefPtr<T>();
		}
	}

	template <class T>
	Gaff::RefPtr<T> requestResourceT(const char8_t* name, bool delay_load = false)
	{
		return requestResourceT<T>(HashStringView64<>(name, eastl::CharStrlen(name)), delay_load);
	}

	IResourcePtr requestResource(const char8_t* name, bool delay_load = false)
	{
		return requestResource(HashStringView64<>(name, eastl::CharStrlen(name)), delay_load);
	}

	template <class T>
	Gaff::RefPtr<T> createResourceT(HashStringView64<> name)
	{
		static_assert(T::Creatable, "Resource is not a creatable type.");
		IResourcePtr old_ptr = createResource(name, Refl::Reflection<T>::GetReflectionDefinition());

		if (old_ptr) {
			return Gaff::RefPtr<T>(static_cast<T*>(old_ptr.release()), false);
		}
		else {
			return Gaff::RefPtr<T>();
		}
	}

	template <class T>
	Gaff::RefPtr<T> createResourceT(const char8_t* name)
	{
		return createResourceT<T>(HashStringView64<>(name, eastl::CharStrlen(name)));
	}

	IResourcePtr createResource(const char8_t* name, const Refl::IReflectionDefinition& ref_def)
	{
		return createResource(HashStringView64<>(name, eastl::CharStrlen(name)), ref_def);
	}

	template <class T>
	Gaff::RefPtr<T> getResourceT(HashStringView64<> name)
	{
		IResourcePtr old_ptr = getResource(name);

		if (old_ptr) {
			return Gaff::RefPtr<T>(static_cast<T*>(old_ptr.release()), false);
		} else {
			return Gaff::RefPtr<T>();
		}
	}

	template <class T>
	Gaff::RefPtr<T> getResourceT(const char8_t* name)
	{
		return getResourceT<T>(HashStringView64<>(name, eastl::CharStrlen(name)));
	}

	IResourcePtr getResource(const char8_t* name)
	{
		return getResource(HashStringView64<>(name, eastl::CharStrlen(name)));
	}

	IResourcePtr createResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def);
	IResourcePtr requestResource(HashStringView64<> name, bool delay_load);
	IResourcePtr requestResource(HashStringView64<> name);
	IResourcePtr getResource(HashStringView64<> name);
	void waitForResource(const IResource& resource) const;

	const IFile* loadFileAndWait(const char8_t* file_path, uintptr_t thread_id_int);

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


template <class T>
static bool LoadRefPtr(const ISerializeReader& reader, Gaff::RefPtr<T>& out)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");

	if (!reader.isString()) {
		return false;
	}

	const char8_t* const res_path = reader.readString();
	out = GetManagerTFast<ResourceManager>().requestResourceT<T>(res_path);
	reader.freeString(res_path);

	return out;
}

template <class T>
static void SaveRefPtr(ISerializeWriter& writer, const Gaff::RefPtr<T>& value)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");
	writer.writeString(value->getFilePath().getBuffer());
}

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ResourceManager)

// Gaff::RefPtr
SHIB_TEMPLATE_REFLECTION_DECLARE(Gaff::RefPtr, T)

SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Gaff::RefPtr, T)
	.serialize(Shibboleth::LoadRefPtr<T>, Shibboleth::SaveRefPtr<T>)
	.func("get", &Gaff::RefPtr<T>::get)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Gaff::RefPtr, T)
