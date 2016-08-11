/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include <Shibboleth_TaskPoolTags.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ScopedLock.h>

NS_SHIBBOLETH

struct ResourceData
{
	IResourceLoader* res_loader;
	ResourcePtr res_ptr;
	unsigned int job_pool;
	IFile* file;
	IResourceManager* res_mgr;
};

void ResourceLoadingJob(void* data)
{
	ResourceData* res_data = reinterpret_cast<ResourceData*>(data);
	ResourceContainer* rc = res_data->res_ptr.get();

	rc->_res_state = ResourceContainer::RS_PENDING;

	ResourceLoadData load_data = res_data->res_loader->load(res_data->file, rc);

	if (!load_data.data) {
		rc->_res_state = ResourceContainer::RS_FAILED;
		rc->callCallbacks();

	} else if (!load_data.sub_res_data.empty()) {
		rc->_resource = load_data.data;

		for (auto it = load_data.sub_res_data.begin(); it != load_data.sub_res_data.end(); ++it) {
			ResourcePtr res_ptr = res_data->res_mgr->requestResource(it->file_name.getBuffer(), it->user_data);
			rc->_sub_resources.emplacePush(res_ptr);
			res_ptr->addCallback(it->callback);
		}

	} else {
		rc->_resource = load_data.data;
		rc->_res_state = ResourceContainer::RS_LOADED;
		rc->callCallbacks();
	}

	if (res_data->file) {
		IFileSystem* fs = GetApp().getFileSystem();
		fs->closeFile(res_data->file);
	}

	SHIB_FREET(res_data, *GetAllocator());
}

void ResourceReadingJob(void* data)
{
	ResourceData* res_data = reinterpret_cast<ResourceData*>(data);
	IFileSystem* fs = GetApp().getFileSystem();
	IFile* file = fs->openFile(res_data->res_ptr->getResourceKey().getString().getBuffer());

	if (!file) {
		GetApp().getLogManager().logMessage(
			LogManager::LOG_ERROR,
			GetApp().getLogFileName(),
			"ERROR - Failed to find or open file '%s'.\n",
			res_data->res_ptr->getResourceKey().getString().getBuffer()
		);

		SHIB_FREET(res_data, *GetAllocator());
		return;
	}

	res_data->file = file;

	Gaff::JobData job_data(&ResourceLoadingJob, data);
	GetApp().getJobPool().addJobs(&job_data, 1, nullptr, res_data->job_pool);
}

REF_IMPL_REQ(ResourceManager);
SHIB_REF_IMPL(ResourceManager)
.addBaseClassInterfaceOnly<ResourceManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IResourceManager)
;

ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager(void)
{
	for (auto it = _resource_cache.begin(); it != _resource_cache.end(); ++it) {
		it.getValue()->_res_mgr = nullptr;
	}

	_resource_cache.clear();
}

const char* ResourceManager::getName(void) const
{
	return GetFriendlyName();
}

void ResourceManager::registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& resource_types, unsigned int thread_pool)
{
	GAFF_ASSERT(res_loader && resource_types.size());

	LoaderData loader_data = { ResourceLoaderPtr(res_loader), thread_pool };

	for (auto it = resource_types.begin(); it != resource_types.end(); ++it) {
		GAFF_ASSERT(_resource_loaders.indexOf(*it) == SIZE_T_FAIL);
		_resource_loaders[*it] = loader_data;
	}
}

void ResourceManager::registerResourceLoader(IResourceLoader* res_loader, const char* resource_type, unsigned int thread_pool)
{
	GAFF_ASSERT(_resource_loaders.indexOf(resource_type) == SIZE_T_FAIL);
	LoaderData loader_data = { ResourceLoaderPtr(res_loader), thread_pool };
	_resource_loaders[resource_type] = loader_data;
}

ResourcePtr ResourceManager::requestResource(const char* resource_type, const char* instance_name, uint64_t user_data)
{
	GAFF_ASSERT(resource_type && strlen(resource_type) && instance_name && strlen(instance_name));
	AHashString res_key(instance_name);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it,
		// so make a load request.
		GAFF_ASSERT(_resource_loaders.indexOf(resource_type) != SIZE_T_FAIL);

		ResourceContainer* res_cont = SHIB_ALLOCT(ResourceContainer, *GetAllocator(), res_key, user_data, this);
		ResourcePtr res_ptr(res_cont);
		_resource_cache[res_key] = res_cont;

		for (auto& callback : _request_added_callbacks) {
			callback(res_ptr);
		}

		// Make load task
		LoaderData& loader_data = _resource_loaders[resource_type];

		ResourceData* res_data = SHIB_ALLOCT(ResourceData, *GetAllocator());
		res_data->res_loader = loader_data.res_loader.get();
		res_data->res_ptr = res_ptr;
		res_data->job_pool = loader_data.job_pool;
		res_data->file = nullptr;
		res_data->res_mgr = this;

		Gaff::JobData job_data(&ResourceLoadingJob, res_data);
		GetApp().getJobPool().addJobs(&job_data, 1, nullptr, loader_data.job_pool);

		return res_ptr;
	}

	// We have a cache of this resource or we've already made a request to load it.
	// Send the container/future back.
	return ResourcePtr(it.getValue());
}

ResourcePtr ResourceManager::requestResource(const char* filename, uint64_t user_data)
{
	GAFF_ASSERT(filename && strlen(filename));
	AHashString res_key(filename);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it,
		// so make a load request.
		AString extension = res_key.getString().getExtension('.');
		GAFF_ASSERT(extension.size() && _resource_loaders.indexOf(extension) != SIZE_T_FAIL);

		ResourceContainer* res_cont = SHIB_ALLOCT(ResourceContainer, *GetAllocator(), res_key, user_data, this);
		ResourcePtr res_ptr(res_cont);
		_resource_cache[res_key] = res_cont;

		for (auto& callback : _request_added_callbacks) {
			callback(res_ptr);
		}

		// Make load task
		LoaderData& loader_data = _resource_loaders[extension];

		ResourceData* res_data = SHIB_ALLOCT(ResourceData, *GetAllocator());
		res_data->res_loader = loader_data.res_loader.get();
		res_data->res_ptr = res_ptr;
		res_data->job_pool = loader_data.job_pool;
		res_data->file = nullptr;
		res_data->res_mgr = this;

		Gaff::JobData job_data(&ResourceReadingJob, res_data);
		GetApp().getJobPool().addJobs(&job_data, 1, nullptr, TPT_IO);

		return res_ptr;
	}

	// We have a cache of this resource or we've already made a request to load it.
	// Send the container/future back.
	return ResourcePtr(it.getValue());
}

void ResourceManager::addRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback)
{
	_request_added_callbacks.emplacePush(callback);
}

void ResourceManager::removeRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback)
{
	auto it = _request_added_callbacks.linearSearch(callback);

	if (it != _request_added_callbacks.end()) {
		_request_added_callbacks.fastErase(it);
	}
}

void ResourceManager::handleZeroRef(const AHashString& res_key)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	GAFF_ASSERT(_resource_cache.indexOf(res_key) != SIZE_T_FAIL);
	_resource_cache.erase(res_key);
}

NS_END
