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

#include "Shibboleth_ResourcePtr.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_VectorMap.h>
#include <Ptrs/Shibboleth_ArrayPtr.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_AppUtils.h>
#include <eathread/eathread_mutex.h>
#include <EASTL/functional.h>

NS_SHIBBOLETH

#ifdef _DEBUG
	class IDebugManager;
#endif

class ResourceManager final : public IManager
{
public:
	using ResourceStateCallback = eastl::function<void (const Vector<IResource*>&)>;

	ResourceManager(void);
	~ResourceManager(void);

	bool initAllModulesLoaded(void) override;


	template <class T, class U = T>
	ResourcePtr<U> requestResourceT(HashStringView64<> name, bool delay_load = false)
	{
		static_assert(std::is_base_of_v<IResource, T>, "T is not derived from IResource.");
		static_assert(std::is_base_of_v<T, U>, "U is not derived from T.");

		ResourcePtr<IResource> resource = requestResource(name, Refl::Reflection<T>::GetReflectionDefinition(), delay_load);
		return ReflectionCast<U>(std::move(resource));
	}

	template <class T, class U = T>
	ResourcePtr<T> requestResourceT(const char8_t* name, bool delay_load = false)
	{
		return requestResourceT<T, U>(HashStringView64<>(name, eastl::CharStrlen(name)), delay_load);
	}

	ResourcePtr<IResource> requestResource(const char8_t* name, const Refl::IReflectionDefinition& ref_def, bool delay_load = false)
	{
		return requestResource(HashStringView64<>(name, eastl::CharStrlen(name)), ref_def, delay_load);
	}

	template <class T, class U = T>
	ResourcePtr<U> createResourceT(HashStringView64<> name)
	{
		static_assert(std::is_base_of_v<IResource, T>, "T is not derived from IResource.");
		static_assert(std::is_base_of_v<T, U>, "U is not derived from T.");
		static_assert(T::Creatable, "Resource is not a creatable type.");

		ResourcePtr<IResource> resource = createResource(name, Refl::Reflection<T>::GetReflectionDefinition());
		return ReflectionCast<U>(std::move(resource));
	}

	template <class T, class U = T>
	ResourcePtr<U> createResourceT(const char8_t* name)
	{
		return createResourceT<T, U>(HashStringView64<>(name, eastl::CharStrlen(name)));
	}

	ResourcePtr<IResource> createResource(const char8_t* name, const Refl::IReflectionDefinition& ref_def)
	{
		return createResource(HashStringView64<>(name, eastl::CharStrlen(name)), ref_def);
	}

	template <class T, class U = T>
	ResourcePtr<U> getResourceT(HashStringView64<> name)
	{
		static_assert(std::is_base_of_v<IResource, T>, "T is not derived from IResource.");
		static_assert(std::is_base_of_v<T, U>, "U is not derived from T.");

		ResourcePtr<IResource> resource = getResource(name, Refl::Reflection<T>::GetReflectionDefinition());
		return ReflectionCast<U>(std::move(resource));
	}

	template <class T, class U = T>
	ResourcePtr<U> getResourceT(const char8_t* name)
	{
		return getResourceT<T>(HashStringView64<>(name, eastl::CharStrlen(name)));
	}

	ResourcePtr<IResource> getResource(const char8_t* name, const Refl::IReflectionDefinition& ref_def)
	{
		return getResource(HashStringView64<>(name, eastl::CharStrlen(name)), ref_def);
	}

	ResourcePtr<IResource> createResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def);
	ResourcePtr<IResource> requestResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def, bool delay_load);
	ResourcePtr<IResource> requestResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def);
	ResourcePtr<IResource> getResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def);

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

	struct ResourceBucket final
	{
		Vector<IResource*> resources{ ProxyAllocator("Resource") };
		EA::Thread::Mutex lock;
		const Refl::IReflectionDefinition* ref_def = nullptr;

		std::strong_ordering operator<=>(const Refl::IReflectionDefinition& rhs) const
		{
			return ref_def <=> &rhs;
		}

		std::strong_ordering operator<=>(const Refl::IReflectionDefinition* rhs) const
		{
			return ref_def <=> rhs;
		}
	};

	ArrayPtr<ResourceBucket> _resource_buckets;

	EA::Thread::Mutex _removal_lock;
	Vector<const IResource*> _pending_removals{ ProxyAllocator("Resource") };

	VectorMap<Gaff::Hash32, FactoryFunc> _resource_factories{ ProxyAllocator("Resource") };

	Vector<Gaff::Hash64> _callback_keys{ ProxyAllocator("Resource") };
	VectorMap<Gaff::Hash64, CallbackData> _callbacks{ ProxyAllocator("Resource") };
	EA::Thread::Mutex _callback_lock;

	ProxyAllocator _allocator{ "Resource" };

	void checkAndRemoveResources(void);
	void checkCallbacks(void);

	void removeResource(const IResource& resource);
	void requestLoad(IResource& resource);

#ifdef _DEBUG
	IDebugManager* _debug_mgr = nullptr;

	void renderLoadedResources(void) const;
#endif

	friend class ResourceSystem;
	friend class IResource;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ResourceManager)
