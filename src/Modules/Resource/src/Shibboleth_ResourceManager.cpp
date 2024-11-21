/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#define SHIB_REFL_IMPL
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_ResourceAttributesCommon.h"
#include "Shibboleth_ResourceLogging.h"
#include <FileSystem/Shibboleth_IFileSystem.h>
#include <Config/Shibboleth_EngineConfig.h>
#include <Log/Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Assert.h>
#include <EASTL/algorithm.h>

#ifdef _DEBUG
	#include <Shibboleth_DebugAttributes.h>
	#include <imgui.h>
#endif

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ResourceManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()

	//.func("requestResource", static_cast<Shibboleth::IResource* (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>, const Refl::IReflectionDefinition&, bool)>(&Shibboleth::ResourceManager::requestResource))
	//.func("requestResource", static_cast<Shibboleth::IResource* (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>, const Refl::IReflectionDefinition&)>(&Shibboleth::ResourceManager::requestResource))
	//.func("createResource", static_cast<Shibboleth::IResource* (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>, const Refl::IReflectionDefinition&)>(&Shibboleth::ResourceManager::createResource))
	//.func("getResource", static_cast<Shibboleth::IResource* (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>, const Refl::IReflectionDefinition&)>(&Shibboleth::ResourceManager::getResource))

#ifdef _DEBUG
	.func(
		"Loaded Resources",
		&Shibboleth::ResourceManager::renderLoadedResources,
		Shibboleth::DebugMenuItemAttribute(u8"Resource", true)
	)
#endif
SHIB_REFLECTION_DEFINE_END(Shibboleth::ResourceManager)


namespace
{
	struct RawJobData final
	{
		const char8_t* file_path;
		const Shibboleth::IFile* out_file;
	};

	static Shibboleth::ProxyAllocator s_allocator{ RESOURCE_ALLOCATOR };

	static void ResourceFileLoadRawJob(uintptr_t /*thread_id_int*/, void* data)
	{
		RawJobData* const job_data = reinterpret_cast<RawJobData*>(data);

		Shibboleth::U8String final_path(s_allocator);
		final_path.sprintf(u8"Resources/%s", job_data->file_path);

		job_data->out_file = Shibboleth::GetApp().getFileSystem().openFile(final_path.data());
	}

	static void ResourceFileLoadJob(uintptr_t /*thread_id_int*/, void* data)
	{
		Shibboleth::IResource* const res = reinterpret_cast<Shibboleth::IResource*>(data);
		res->load();
	}

	static bool ResourceHashCompare(const Shibboleth::IResource* lhs, Gaff::Hash64 rhs)
	{
		return lhs->getFilePath().getHash() < rhs;
	}
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ResourceManager)

ResourceManager::~ResourceManager(void)
{
	for (const ResourceBucket& bucket : _resource_buckets) {
		for (const IResource* res : bucket.resources) {
			LogWarningResource("Resource Leaked: %s", res->getFilePath().getBuffer());
		}
	}
}

bool ResourceManager::initAllModulesLoaded(void)
{
	IApp& app = GetApp();

	app.getLogManager().addChannel(HashStringView32<>(k_log_channel_name_resource));

	ReflectionManager& refl_mgr = app.getReflectionManager();
	const auto* const type_bucket = refl_mgr.getTypeBucket(Refl::Reflection<IResource>::GetNameHash());

	if (!type_bucket) {
		return true;
	}

	_resource_buckets = ArrayPtr<ResourceBucket>(SHIB_ALLOC_ARRAYT(ResourceBucket, type_bucket->size(), s_allocator), type_bucket->size());

	Vector<const Refl::IReflectionDefinition*> sorted_type_bucket = *type_bucket;
	Vector<const ResourceExtensionAttribute*> ext_attrs;
	const CreatableAttribute* creatable = nullptr;

	Gaff::Sort(sorted_type_bucket);

	for (int32_t i = 0; i < static_cast<int32_t>(sorted_type_bucket.size()); ++i) {
		const Refl::IReflectionDefinition* const ref_def = sorted_type_bucket[i];
		FactoryFunc factory_func = ref_def->template getFactory<>();

		creatable = ref_def->getClassAttr<CreatableAttribute>();
		ref_def->getClassAttrs(ext_attrs);

		GAFF_ASSERT_MSG(factory_func, "Resource '%s' does not have a default constructor!", ref_def->getReflectionInstance().getName());
		GAFF_ASSERT_MSG(creatable || !ext_attrs.empty(), "Resource '%s' is not creatable and does not have any ResourceExtensionAttributes!", ref_def->getReflectionInstance().getName());

		for (const ResourceExtensionAttribute* ext_attr : ext_attrs) {
			GAFF_ASSERT_MSG(
				_resource_factories.find(ext_attr->getExtension().getHash()) == _resource_factories.end(),
				"File extension '%s' already has a resource associated with it!",
				ext_attr->getExtension().getBuffer()
			);

			_resource_factories[ext_attr->getExtension().getHash()] = factory_func;
		}

		creatable = nullptr;
		ext_attrs.clear();

		_resource_buckets[i].ref_def = ref_def;
	}

#ifdef _DEBUG
	_debug_mgr = &GETMANAGERT(Shibboleth::IDebugManager, Shibboleth::DebugManager);
#endif

	return true;
}

DeferredResourcePtr<IResource> ResourceManager::requestDeferredResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def)
{
	return DeferredResourcePtr<IResource>{ requestResource(name, ref_def, false) };
}

ResourcePtr<IResource> ResourceManager::requestResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def)
{
	return ResourcePtr<IResource>{ requestResource(name, ref_def, false) };
}

ResourcePtr<IResource> ResourceManager::createResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def)
{
	if (!ref_def.getClassAttr<CreatableAttribute>()) {
		LogErrorResource("Resource type '%s' is not creatable.", ref_def.getReflectionInstance().getName());
		return ResourcePtr<IResource>();
	}

	const auto it_bucket = Gaff::LowerBound(_resource_buckets, &ref_def);
	GAFF_ASSERT(it_bucket != _resource_buckets.end() && it_bucket->ref_def == &ref_def);

	const EA::Thread::AutoMutex lock(it_bucket->lock);
	const auto it_res = Gaff::LowerBound(it_bucket->resources, name.getHash(), ResourceHashCompare);

	if (it_res != it_bucket->resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return ResourcePtr<IResource>(*it_res);
	}

	// create instance
	const auto factory = ref_def.getFactory<>();

	if (!factory) {
		LogErrorResource("Resource type '%s' does not have a default constructor.", ref_def.getReflectionInstance().getName());
		return ResourcePtr<IResource>();
	}

	void* const data = factory(s_allocator);
	IResource* const resource = ref_def.GET_INTERFACE(Shibboleth::IResource, data);
	resource->_state = ResourceState::Loaded;
	resource->_file_path = name;
	resource->_res_mgr = this;

	it_bucket->resources.insert(it_res, resource);

	return ResourcePtr<IResource>(resource);
}

ResourcePtr<IResource> ResourceManager::getResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def)
{
	const auto it_bucket = Gaff::LowerBound(_resource_buckets, &ref_def);
	GAFF_ASSERT(it_bucket != _resource_buckets.end() && it_bucket->ref_def == &ref_def);

	const EA::Thread::AutoMutex lock(it_bucket->lock);
	const auto it_res = Gaff::LowerBound(it_bucket->resources, name.getHash(), ResourceHashCompare);

	if (it_res != it_bucket->resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return ResourcePtr<IResource>(*it_res);
	}

	return ResourcePtr<IResource>();
}

void ResourceManager::waitForResource(const IResource& resource) const
{
	while (resource._state == ResourceState::Pending) {
		// $TODO: Help out?
		EA::Thread::ThreadSleep();
	}
}

const IFile* ResourceManager::loadFileAndWait(const char8_t* file_path, uintptr_t thread_id_int)
{
	RawJobData data = { file_path, nullptr };
	Gaff::JobData job_data = { ResourceFileLoadRawJob, &data };
	Gaff::Counter counter = 0;
	auto& job_pool = GetApp().getJobPool();

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	job_pool.addJobs(&job_data, 1, counter, EngineConfig::k_read_file_pool);
	job_pool.helpWhileWaiting(thread_id, counter);

	return data.out_file;
}

ResourceCallbackID ResourceManager::registerCallback(const Vector<const IResource*>& resources, const ResourceStateCallback& callback)
{
	GAFF_ASSERT(Gaff::Find(resources, nullptr) == resources.end());
	bool already_loaded = true;

	for (const IResource* res : resources) {
		if (res->getState() == ResourceState::Pending) {
			already_loaded = false;
			break;
		}
	}

	if (already_loaded) {
		callback(resources);
		return ResourceCallbackID{};
	}

	const Gaff::Hash64 hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(resources.data()), sizeof(IResource*) * resources.size());
	const EA::Thread::AutoMutex lock(_callback_lock);
	CallbackData& data = _callbacks[hash];

	if (data.resources.empty()) {
		data.resources = *reinterpret_cast<Vector<IResource*>*>(const_cast<Vector<const IResource*>*>(&resources));
	}

	ResourceCallbackID id{ hash, data.next_id++ };

	data.callbacks[id.cb_id] = callback;

	return id;
}

ResourceCallbackID ResourceManager::registerCallback(const IResource& resource, const ResourceStateCallback& callback)
{
	const Vector<const IResource*> resource_vector{ { &resource }, s_allocator };
	return registerCallback(resource_vector, callback);
}

void ResourceManager::removeCallback(ResourceCallbackID id)
{
	const EA::Thread::AutoMutex lock(_callback_lock);
	const auto it = _callbacks.find(id.res_id);

	if (it == _callbacks.end()) {
		return;
	}

	it->second.callbacks.erase(id.cb_id);

	if (it->second.callbacks.empty()) {
		_callbacks.erase(it);
	}
}

const Vector<const IResource*>& ResourceManager::getResources(const Refl::IReflectionDefinition& ref_def) const
{
	const Vector<IResource*>& resources = const_cast<ResourceManager*>(this)->getResources(ref_def);

	// Only adding const qualifier to element type, should be safe to reinterpret_cast.
	return reinterpret_cast<const Vector<const IResource*>&>(resources);
}

const Vector<IResource*>& ResourceManager::getResources(const Refl::IReflectionDefinition& ref_def)
{
	const auto it_bucket = Gaff::LowerBound(_resource_buckets, &ref_def);
	GAFF_ASSERT(it_bucket != _resource_buckets.end() && it_bucket->ref_def == &ref_def);

	return it_bucket->resources;
}

EA::Thread::Mutex& ResourceManager::getResourceBucketLock(const Refl::IReflectionDefinition& ref_def)
{
	const auto it_bucket = Gaff::LowerBound(_resource_buckets, &ref_def);
	GAFF_ASSERT(it_bucket != _resource_buckets.end() && it_bucket->ref_def == &ref_def);

	return it_bucket->lock;
}

IResource* ResourceManager::requestResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def, bool delay_load)
{
	const auto it_bucket = Gaff::LowerBound(_resource_buckets, &ref_def);
	GAFF_ASSERT(it_bucket != _resource_buckets.end() && it_bucket->ref_def == &ref_def);

	const EA::Thread::AutoMutex lock(it_bucket->lock);
	const auto it_res = Gaff::LowerBound(it_bucket->resources, name.getHash(), ResourceHashCompare);

	if (it_res != it_bucket->resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return *it_res;
	}

	const size_t pos = Gaff::ReverseFind(name.getBuffer(), u8'.');

	if (pos == GAFF_SIZE_T_FAIL) {
		// $TODO: Log error
		return nullptr;
	}

	// Search for res_factory that handles this resource file.
	const Gaff::Hash32 res_file_hash = Gaff::FNV1aHash32String(name.getBuffer() + pos);
	const auto it_fact = _resource_factories.find(res_file_hash);

	if (it_fact == _resource_factories.end()) {
		// $TODO: Log error
		return nullptr;
	}

	// Assume all resources always inherit from IResource first.
	IResource* const resource = static_cast<IResource*>(it_fact->second(s_allocator));
	resource->_file_path = name;
	resource->_res_mgr = this;

	it_bucket->resources.insert(it_res, resource);

	if (!delay_load) {
		requestLoad(*resource);
	}

	return resource;
}

void ResourceManager::checkAndRemoveResources(void)
{
	{
		const EA::Thread::AutoMutex removal_lock(_removal_lock);

		for (int32_t i = 0; i < static_cast<int32_t>(_pending_removals.size());) {
			const IResource* const resource = _pending_removals[i];

			// Don't remove if a thread is still loading it.
			if (resource->getState() == ResourceState::Pending) {
				++i;

			} else {
				_pending_removals.erase_unsorted(_pending_removals.begin() + i);

				// Resource gained a reference since pending removal.
				if (resource->getRefCount() > 0) {
					continue;
				}

				// Get resource bucket.
				const Refl::IReflectionDefinition& ref_def = resource->getReflectionDefinition();
				const auto it_bucket = Gaff::LowerBound(_resource_buckets, &ref_def);
				GAFF_ASSERT(it_bucket != _resource_buckets.end() && it_bucket->ref_def == &ref_def);

				// Remove resource from resource bucket.
				const EA::Thread::AutoMutex bucket_lock(it_bucket->lock);
				const auto it_res = Gaff::LowerBound(it_bucket->resources, resource->getFilePath().getHash(), ResourceHashCompare);

				GAFF_ASSERT(it_res != it_bucket->resources.end() && *it_res == resource);

				it_bucket->resources.erase(it_res);
				SHIB_FREET(resource, s_allocator);

				// Remove all callbacks that involve this resource.
				const EA::Thread::AutoMutex callback_lock(_callback_lock);

				for (int32_t j = 0; j < static_cast<int32_t>(_callbacks.size());) {
					const auto it = _callbacks.begin() + j;

					if (Gaff::Contains(it->second.resources, resource)) {
						_callbacks.erase(it);
						continue;
					}

					++j;
				}
			}
		}
	}

	{
		const EA::Thread::AutoMutex unload_lock(_unload_lock);

		for (IResource* resource : _pending_unloads) {
			// Resource gained a load request since unload was requested.
			if (resource->_load_count > 0) {
				continue;
			}

			const Refl::IReflectionDefinition& ref_def = resource->getReflectionDefinition();
			const HashString64<> file_path = resource->_file_path;
			const int32_t ref_count = resource->_ref_count;

			// "unload" the resource be calling the destructor and then calling placement new.
			Gaff::Deconstruct(resource);
			ref_def.template construct<>(resource->getBasePointer());

			resource->_file_path = file_path;
			resource->_ref_count = ref_count;
			resource->_res_mgr = this;
		}

		_pending_unloads.clear();
	}
}

void ResourceManager::checkCallbacks(void)
{
	for (const auto& pair : _callbacks) {
		_callback_keys.emplace_back(pair.first);
	}

	for (Gaff::Hash64 hash : _callback_keys) {
		auto& cb_data = _callbacks[hash];
		bool not_loaded = false;

		for (const IResource* res : cb_data.resources) {
			if (res->getState() == ResourceState::Pending) {
				not_loaded = true;
				break;
			}
		}

		if (not_loaded) {
			continue;
		}

		for (auto& cb : cb_data.callbacks) {
			cb.second(*reinterpret_cast<const Vector<const IResource*>*>(&cb_data.resources));
		}

		_callbacks.erase(hash);
	}

	_callback_keys.clear();
}

void ResourceManager::removeResource(const IResource& resource)
{
	const EA::Thread::AutoMutex lock(_removal_lock);
	_pending_removals.emplace_back(&resource);
}

void ResourceManager::requestUnload(IResource& resource)
{
	const EA::Thread::AutoMutex lock(_unload_lock);
	_pending_unloads.emplace_back(&resource);
}

void ResourceManager::requestLoad(IResource& resource)
{
	// $TODO: Assert instead? Only internal APIs are calling this.
	if (!resource.isDeferred()) {
		LogErrorResource(
			"Call to ResourceManager::requestLoad() called on resource '%s' and is not marked for deferred load.",
			resource._file_path.getBuffer()
		);

		return;
	}

	resource._state = ResourceState::Pending;
	Gaff::JobData job_data = { ResourceFileLoadJob, &resource };
	GetApp().getJobPool().addJobs(&job_data, 1, nullptr, EngineConfig::k_read_file_pool);
}

#ifdef _DEBUG
void ResourceManager::renderLoadedResources(void) const
{
}
#endif

NS_END
