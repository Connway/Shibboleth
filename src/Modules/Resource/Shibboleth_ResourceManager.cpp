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

#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_ResourceAttributesCommon.h"
#include <Shibboleth_ReflectionInterfaces.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_IApp.h>
#include <Gaff_Assert.h>
#include <EASTL/algorithm.h>

SHIB_REFLECTION_DEFINE_BEGIN(ResourceManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(ResourceManager)

SHIB_REFLECTION_DEFINE_BEGIN(ResourceSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(ResourceSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ResourceManager)
SHIB_REFLECTION_CLASS_DEFINE(ResourceSystem)

struct RawJobData final
{
	const char* file_path;
	const IFile* out_file;
};

static void ResourceFileLoadRawJob(void* data)
{
	RawJobData* job_data = reinterpret_cast<RawJobData*>(data);

	char temp[1024] = { 0 };
	snprintf(temp, 1024, "Resources/%s", job_data->file_path);

	job_data->out_file = GetApp().getFileSystem().openFile(temp);
}

static void ResourceFileLoadJob(void* data)
{
	IResource* res = reinterpret_cast<IResource*>(data);
	res->load();
}


ResourceManager::ResourceManager(void)
{
	IApp& app = GetApp();
	app.getReflectionManager().registerTypeBucket(Gaff::FNV1aHash64Const("IResource"));
	app.getLogManager().addChannel("Resource", "ResourceLog");
}

void ResourceManager::allModulesLoaded(void)
{
	IApp& app = GetApp();
	const ReflectionManager& refl_mgr = app.getReflectionManager();
	const Vector<const Gaff::IReflectionDefinition*>* type_bucket = refl_mgr.getTypeBucket(Gaff::FNV1aHash64Const("IResource"));
	GAFF_ASSERT(type_bucket);

	Vector<const ResExtAttribute*> ext_attrs;
	const CreatableAttribute* creatable = nullptr;

	for (const Gaff::IReflectionDefinition* ref_def : *type_bucket) {
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
}

IResourcePtr ResourceManager::createResource(Gaff::HashStringTemp64 name, const Gaff::IReflectionDefinition& ref_def)
{
	if (!ref_def.getClassAttr<CreatableAttribute>()) {
		LogErrorResource("Resource type '%s' is not creatable.", ref_def.getReflectionInstance().getName());
		return IResourcePtr();
	}

	EA::Thread::AutoMutex lock(_res_lock);

	auto it_res = eastl::lower_bound(
		_resources.begin(),
		_resources.end(),
		name.getHash(),
		[](const IResource* lhs, Gaff::Hash64 rhs) -> bool
		{
			return lhs->getFilePath().getHash() < rhs;
		}
	);

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
	IResource* resource = ref_def.GET_INTERFACE(IResource, data);
	resource->_file_path = name;
	resource->_res_mgr = this;

	_resources.insert(it_res, resource);

	return IResourcePtr(resource);
}

IResourcePtr ResourceManager::requestResource(Gaff::HashStringTemp64 name, bool delay_load)
{
	EA::Thread::AutoMutex lock(_res_lock);

	auto it_res = eastl::lower_bound(
		_resources.begin(),
		_resources.end(),
		name.getHash(),
		[](const IResource* lhs, Gaff::Hash64 rhs) -> bool
		{
			return lhs->getFilePath().getHash() < rhs;
		}
	);

	if (it_res != _resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return IResourcePtr(*it_res);
	}

	size_t pos = Gaff::FindLastOf(name.getBuffer(), '.');

	if (pos == SIZE_T_FAIL) {
		// $TODO: Log error
		return IResourcePtr();
	}

	// Search for res_factory that handles this resource file.
	Gaff::Hash32 res_file_hash = Gaff::FNV1aHash32String(name.getBuffer() + pos);
	auto it_fact = _resource_factories.find(res_file_hash);

	if (it_fact == _resource_factories.end()) {
		// $TODO: Log error
		return IResourcePtr();
	}

	// Assume all resources always inherit from IResource first.
	IResource* res = reinterpret_cast<IResource*>(it_fact->second(_allocator));
	res->_file_path = name;
	res->_res_mgr = this;

	_resources.insert(it_res, res);

	if (!delay_load) {
		requestLoad(*res);
	}

	return IResourcePtr(res);
}

IResourcePtr ResourceManager::getResource(Gaff::HashStringTemp64 name)
{
	EA::Thread::AutoMutex lock(_res_lock);

	auto it_res = eastl::lower_bound(
		_resources.begin(),
		_resources.end(),
		name.getHash(),
		[](const IResource* lhs, Gaff::Hash64 rhs) -> bool
	{
		return lhs->getFilePath().getHash() < rhs;
	}
	);

	if (it_res != _resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return IResourcePtr(*it_res);
	}

	return IResourcePtr();
}

void ResourceManager::waitForResource(const IResource& resource) const
{
	while (resource._state == IResource::RS_PENDING) {
		// $TODO: Help out?
		EA::Thread::ThreadSleep();
	}
}

const IFile* ResourceManager::loadFileAndWait(const char* file_path)
{
	RawJobData data = { file_path, nullptr };
	Gaff::JobData job_data = { ResourceFileLoadRawJob, &data };
	Gaff::Counter counter = 0;
	auto& job_pool = GetApp().getJobPool();

	job_pool.addJobs(&job_data, 1, counter, JPI_READ_FILE);
	job_pool.helpWhileWaiting(counter);

	return data.out_file;
}

ResourceCallbackID ResourceManager::registerCallback(const Vector<IResource*>& resources, const ResourceStateCallback& callback)
{
	bool already_loaded = true;

	for (IResource* res : resources) {
		if (res->getState() == IResource::RS_PENDING) {
			already_loaded = false;
			break;
		}
	}

	if (already_loaded) {
		callback(resources);
		return ResourceCallbackID{ Gaff::INIT_HASH64, -1 };
	}

	const Gaff::Hash64 hash = Gaff::FNV1aHash64(reinterpret_cast<const char*>(resources.data()), sizeof(IResource*) * resources.size());
	EA::Thread::AutoMutex lock(_callback_lock);
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
	EA::Thread::AutoMutex lock(_callback_lock);
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
	EA::Thread::AutoMutex lock(_removal_lock);

	for (int32_t i = 0; i < static_cast<int32_t>(_pending_removals.size());) {
		if (_pending_removals[i]->getState() == IResource::RS_PENDING) {
			++i;
		} else {
			_pending_removals.erase_unsorted(_pending_removals.begin() + i);
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
			if (res->getState() == IResource::RS_PENDING) {
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
	if (resource.getState() == IResource::RS_PENDING) {
		EA::Thread::AutoMutex lock(_removal_lock);
		_pending_removals.emplace_back(&resource);

	} else {
		EA::Thread::AutoMutex lock(_res_lock);

		auto it_res = eastl::lower_bound(_resources.begin(), _resources.end(), &resource);

		if (it_res != _resources.end() && *it_res == &resource) {
			_resources.erase(it_res);
		}
	}
}

void ResourceManager::requestLoad(IResource& resource)
{
	if (resource._state != IResource::RS_DELAYED) {
		LogErrorResource(
			"Call to ResourceManager::requestLoad() called on resource '%s' and is not marked for delayed load.",
			resource._file_path.getBuffer()
		);

		return;
	}

	resource._state = IResource::RS_PENDING;
	Gaff::JobData job_data = { ResourceFileLoadJob, &resource };
	GetApp().getJobPool().addJobs(&job_data, 1, nullptr, JPI_READ_FILE);
}



bool ResourceSystem::init(void)
{
	_res_mgr = &GetApp().getManagerTFast<ResourceManager>();
	return true;
}

void ResourceSystem::update()
{
	_res_mgr->checkCallbacks();
	_res_mgr->checkAndRemoveResources();
}

NS_END
