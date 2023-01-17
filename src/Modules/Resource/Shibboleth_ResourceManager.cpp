/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_ResourceAttributesCommon.h"
#include "Shibboleth_ResourceLogging.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_AppConfigs.h>
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

	.func("requestResource", static_cast<Shibboleth::IResourcePtr (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>, bool)>(&Shibboleth::ResourceManager::requestResource))
	.func("requestResource", static_cast<Shibboleth::IResourcePtr (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>)>(&Shibboleth::ResourceManager::requestResource))
	//.func("createResource", static_cast<Shibboleth::IResourcePtr (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>, const Refl::IReflectionDefinition&)>(&Shibboleth::ResourceManager::createResource))
	.func("getResource", static_cast<Shibboleth::IResourcePtr (Shibboleth::ResourceManager::*)(Shibboleth::HashStringView64<>)>(&Shibboleth::ResourceManager::getResource))

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
	static constexpr Gaff::Hash32 k_read_file_pool = Gaff::FNV1aHash32StringConst(Shibboleth::k_config_app_read_file_pool_name);

	struct RawJobData final
	{
		const char8_t* file_path;
		const Shibboleth::IFile* out_file;
	};

	static void ResourceFileLoadRawJob(uintptr_t /*id_int*/, void* data)
	{
		RawJobData* const job_data = reinterpret_cast<RawJobData*>(data);

		Shibboleth::U8String final_path(Shibboleth::ProxyAllocator("Resource"));
		final_path.sprintf(u8"Resources/%s", job_data->file_path);

		job_data->out_file = Shibboleth::GetApp().getFileSystem().openFile(final_path.data());
	}

	static void ResourceFileLoadJob(uintptr_t /*id_int*/, void* data)
	{
		Shibboleth::IResource* res = reinterpret_cast<Shibboleth::IResource*>(data);
		res->load();
	}

	static bool ResourceHashCompare(const Shibboleth::IResource* lhs, Gaff::Hash64 rhs)
	{
		return lhs->getFilePath().getHash() < rhs;
	}
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ResourceManager)

ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
	for (const IResource* res : _resources) {
		LogWarningResource("Resource Leaked: %s", res->getFilePath().getBuffer());
	}
}

bool ResourceManager::init(void)
{
	IApp& app = GetApp();
	ReflectionManager& refl_mgr = app.getReflectionManager();

	refl_mgr.registerTypeBucket(Refl::Reflection<IResource>::GetHash());
	app.getLogManager().addChannel(HashStringView32<>(k_log_channel_name_resource));

	const Vector<const Refl::IReflectionDefinition*>* type_bucket =
		refl_mgr.getTypeBucket(Refl::Reflection<IResource>::GetHash());

	if (!type_bucket) {
		return true;
	}

	Vector<const ResExtAttribute*> ext_attrs;
	const CreatableAttribute* creatable = nullptr;

	for (const Refl::IReflectionDefinition* ref_def : *type_bucket) {
		FactoryFunc factory_func = ref_def->template getFactory<>();

		creatable = ref_def->getClassAttr<CreatableAttribute>();
		ref_def->getClassAttrs(ext_attrs);

		GAFF_ASSERT_MSG(factory_func, "Resource '%s' does not have a default constructor!", ref_def->getReflectionInstance().getName());
		GAFF_ASSERT_MSG(creatable || !ext_attrs.empty(), "Resource '%s' is not creatable and does not have any ResExtAttribute's!", ref_def->getReflectionInstance().getName());

		for (const ResExtAttribute* ext_attr : ext_attrs) {
			GAFF_ASSERT_MSG(
				_resource_factories.find(ext_attr->getExtension().getHash()) == _resource_factories.end(),
				"File extension '%s' already has a resource associated with it!",
				ext_attr->getExtension().getBuffer()
			);

			_resource_factories[ext_attr->getExtension().getHash()] = factory_func;
		}

		creatable = nullptr;
		ext_attrs.clear();
	}

#ifdef _DEBUG
	_debug_mgr = &GETMANAGERT(Shibboleth::IDebugManager, Shibboleth::DebugManager);
#endif

	return true;
}

IResourcePtr ResourceManager::createResource(HashStringView64<> name, const Refl::IReflectionDefinition& ref_def)
{
	if (!ref_def.getClassAttr<CreatableAttribute>()) {
		LogErrorResource("Resource type '%s' is not creatable.", ref_def.getReflectionInstance().getName());
		return IResourcePtr();
	}

	const EA::Thread::AutoMutex lock(_res_lock);

	const auto it_res = Gaff::LowerBound(_resources, name.getHash(), ResourceHashCompare);

	if (it_res != _resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return IResourcePtr(*it_res);
	}

	// create instance
	const auto factory = ref_def.getFactory<>();

	if (!factory) {
		LogErrorResource("Resource type '%s' does not have a default constructor.", ref_def.getReflectionInstance().getName());
		return IResourcePtr();
	}

	void* const data = factory(_allocator);
	IResource* const resource = ref_def.GET_INTERFACE(Shibboleth::IResource, data);
	resource->_state = ResourceState::Loaded;
	resource->_file_path = name;
	resource->_res_mgr = this;

	_resources.insert(it_res, resource);

	return IResourcePtr(resource);
}

IResourcePtr ResourceManager::requestResource(HashStringView64<> name, bool delay_load)
{
	const EA::Thread::AutoMutex lock(_res_lock);

	const auto it_res = Gaff::LowerBound(_resources, name.getHash(), ResourceHashCompare);

	if (it_res != _resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return IResourcePtr(*it_res);
	}

	const size_t pos = Gaff::ReverseFind(name.getBuffer(), u8'.');

	if (pos == SIZE_T_FAIL) {
		// $TODO: Log error
		return IResourcePtr();
	}

	// Search for res_factory that handles this resource file.
	const Gaff::Hash32 res_file_hash = Gaff::FNV1aHash32String(name.getBuffer() + pos);
	const auto it_fact = _resource_factories.find(res_file_hash);

	if (it_fact == _resource_factories.end()) {
		// $TODO: Log error
		return IResourcePtr();
	}

	// Assume all resources always inherit from IResource first.
	IResource* const resource = reinterpret_cast<IResource*>(it_fact->second(_allocator));
	resource->_file_path = name;
	resource->_res_mgr = this;

	_resources.insert(it_res, resource);

	if (!delay_load) {
		requestLoad(*resource);
	}

	return IResourcePtr(resource);
}

IResourcePtr ResourceManager::requestResource(HashStringView64<> name)
{
	return requestResource(name, false);
}

IResourcePtr ResourceManager::getResource(HashStringView64<> name)
{
	const EA::Thread::AutoMutex lock(_res_lock);

	const auto it_res = Gaff::LowerBound(_resources, name.getHash(), ResourceHashCompare);

	if (it_res != _resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return IResourcePtr(*it_res);
	}

	return IResourcePtr();
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

	job_pool.addJobs(&job_data, 1, counter, k_read_file_pool);
	job_pool.helpWhileWaiting(thread_id, counter);

	return data.out_file;
}

ResourceCallbackID ResourceManager::registerCallback(const Vector<IResource*>& resources, const ResourceStateCallback& callback)
{
	GAFF_ASSERT(Gaff::Find(resources, nullptr) == resources.end());
	bool already_loaded = true;

	for (IResource* res : resources) {
		if (res->getState() == ResourceState::Pending) {
			already_loaded = false;
			break;
		}
	}

	if (already_loaded) {
		callback(resources);
		return ResourceCallbackID{ Gaff::k_init_hash64, -1 };
	}

	const Gaff::Hash64 hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(resources.data()), sizeof(IResource*) * resources.size());
	const EA::Thread::AutoMutex lock(_callback_lock);
	CallbackData& data = _callbacks[hash];

	if (data.resources.empty()) {
		data.resources = resources;
	}

	ResourceCallbackID id{ hash, data.next_id++ };

	data.callbacks[id.cb_id] = callback;

	return id;
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

void ResourceManager::checkAndRemoveResources(void)
{
	const EA::Thread::AutoMutex pending_lock(_removal_lock);

	for (int32_t i = 0; i < static_cast<int32_t>(_pending_removals.size());) {
		const IResource* const resource = _pending_removals[i];

		// Don't remove if a thread is still loading it.
		if (resource->getState() == ResourceState::Pending) {
			++i;

		} else {
			_pending_removals.erase_unsorted(_pending_removals.begin() + i);

			// Resource gained a reference since pending removal.
			if (resource->getRefCount() > 0) {
				return;
			}

			// Remove and free the resource.
			const EA::Thread::AutoMutex res_lock(_res_lock);
			const auto it_res = Gaff::LowerBound(_resources, resource->getFilePath().getHash(), ResourceHashCompare);

			if (it_res != _resources.end() && *it_res == resource) {
				_resources.erase(it_res);
			}

			SHIB_FREET(resource, GetAllocator());
		}
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

		for (IResource* res : cb_data.resources) {
			if (res->getState() == ResourceState::Pending) {
				not_loaded = true;
				break;
			}
		}

		if (not_loaded) {
			continue;
		}

		for (auto& cb : cb_data.callbacks) {
			cb.second(cb_data.resources);
		}

		_callbacks.erase(hash);
	}

	_callback_keys.clear();
}

void ResourceManager::removeResource(const IResource& resource)
{
	// Resource load job has already been submitted. Wait until it is finished.
	if (resource.getState() == ResourceState::Pending) {
		const EA::Thread::AutoMutex lock(_removal_lock);
		_pending_removals.emplace_back(&resource);

	} else {
		const EA::Thread::AutoMutex lock(_res_lock);

		const auto it_res = Gaff::LowerBound(_resources, resource.getFilePath().getHash(), ResourceHashCompare);

		if (it_res != _resources.end() && *it_res == &resource) {
			_resources.erase(it_res);
			SHIB_FREET(&resource, GetAllocator());
		}
	}
}

void ResourceManager::requestLoad(IResource& resource)
{
	if (resource._state != ResourceState::Delayed) {
		LogErrorResource(
			"Call to ResourceManager::requestLoad() called on resource '%s' and is not marked for delayed load.",
			resource._file_path.getBuffer()
		);

		return;
	}

	resource._state = ResourceState::Pending;
	Gaff::JobData job_data = { ResourceFileLoadJob, &resource };
	GetApp().getJobPool().addJobs(&job_data, 1, nullptr, k_read_file_pool);
}

#ifdef _DEBUG
void ResourceManager::renderLoadedResources(void) const
{
}
#endif

NS_END
