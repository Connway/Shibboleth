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
#include <Gaff_IVirtualDestructor.h>
#include <Gaff_ScopedExit.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_SharedPtr.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

struct ResourceData
{
	// Resource Reading
	const Array<ResourceManager::JSONModifiers>* json_elements; // Lists JSON elements that specify files to read.
	ResourceLoaderPtr res_loader;
	ResourcePtr res_ptr;
	unsigned int job_pool;

	// Resource Loading
	HashMap<AString, IFile*> file_map;
};

void ResourceLoadingJob(void* data)
{
	ResourceData* load_data = reinterpret_cast<ResourceData*>(data);
	load_data->res_ptr->_res_state = ResourceContainer::RS_PENDING;

	Gaff::IVirtualDestructor* res_data = load_data->res_loader->load(load_data->res_ptr->getResourceKey().getString().getBuffer(), load_data->res_ptr->getUserData(), load_data->file_map);

	if (res_data) {
		load_data->res_ptr->setResource(res_data);
		load_data->res_ptr->_res_state = ResourceContainer::RS_LOADED;
	} else {
		// Log error
		load_data->res_ptr->_res_state = ResourceContainer::RS_FAILED;
	}

	load_data->res_ptr->callCallbacks();
	GetAllocator()->freeT(load_data);
}

void ResourceReadingJob(void* data)
{
	ResourceData* read_data = reinterpret_cast<ResourceData*>(data);
	IFileSystem* fs = GetApp().getFileSystem();
	IFile* file = fs->openFile(read_data->res_ptr->getResourceKey().getString().getBuffer());
	HashMap<AString, IFile*>& file_map = read_data->file_map;
	bool failed = false;

	GAFF_SCOPE_EXIT([&](void)
	{
		if (failed) {
			for (auto it = file_map.begin(); it != file_map.end(); ++it) {
				GetApp().getFileSystem()->closeFile(*it);
			}

			read_data->res_ptr->_res_state = ResourceContainer::RS_FAILED;
			read_data->res_ptr->callCallbacks();

			GetAllocator()->freeT(read_data);
		}
	});

	if (file) {
		file_map[read_data->res_ptr->getResourceKey().getString()] = file;

		// If we have specified JSON elements that reference file names, then the file we just loaded
		// is a JSON file. Parse it and load the other files referenced in it.
		if (!read_data->json_elements->empty()) {
			Gaff::JSON json;

			if (!json.parse(file->getBuffer())) {
				failed = true;
				return;
			}

			for (auto it = read_data->json_elements->begin(); it != read_data->json_elements->end(); ++it) {
				Gaff::JSON file_name = json.getObject(it->json_element.getBuffer());

				assert(file_name.isString() || it->optional);

				if (!file_name.isString()) {
					continue;
				}

				AString final_name = AString(file_name.getString()) + it->append_to_filename;

				file = fs->openFile(final_name.getBuffer());

				if (!file) {
					LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to find or open file '%s'.\n", final_name.getBuffer());
					failed = true;
					return;
				}

				file_map[final_name] = file;
			}
		}

		// Create ResourceLoadingJob
		Gaff::JobData job_data(&ResourceLoadingJob, data);
		GetApp().getJobPool().addJobs(&job_data);

	} else {
		failed = true;
	}
}

// Resource Container
ResourceContainer::ResourceContainer(const AHashString& res_key, ResourceManager* res_manager, ZRC zero_ref_callback, uint64_t user_data):
	_res_manager(res_manager), _zero_ref_callback(zero_ref_callback), _res_key(res_key),
	_resource(nullptr), _user_data(user_data), _ref_count(0), _res_state(RS_NONE)
{
}

ResourceContainer::~ResourceContainer(void)
{
}

void ResourceContainer::addRef(void) const
{
	AtomicIncrement(&_ref_count);
}

void ResourceContainer::release(void) const
{
	unsigned int new_count = AtomicDecrement(&_ref_count);

	if (!new_count) {
		// Tell ResourceManager that the client no longer has references to this resource, so remove it from your list.
		(_res_manager->*_zero_ref_callback)(_res_key);

		if (_resource) {
			Shibboleth::GetAllocator()->freeT(_resource);
		}

		Shibboleth::GetAllocator()->freeT(this);
	}
}

unsigned int ResourceContainer::getRefCount(void) const
{
	return _ref_count;
}

uint64_t ResourceContainer::getUserData(void) const
{
	return _user_data;
}

const AHashString& ResourceContainer::getResourceKey(void) const
{
	return _res_key;
}

ResourceContainer::ResourceState ResourceContainer::getResourceState(void) const
{
	return _res_state;
}

bool ResourceContainer::isLoaded(void) const
{
	return _res_state == RS_LOADED;
}

bool ResourceContainer::isPending(void) const
{
	return _res_state == RS_PENDING;
}

bool ResourceContainer::hasFailed(void) const
{
	return _res_state == RS_FAILED;
}

void ResourceContainer::addCallback(const Gaff::FunctionBinder<void, ResourceContainer*>& callback)
{
	// We're already loaded!
	if (_resource != nullptr) {
		callback(this);
		return;
	}

	Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);
	_callbacks.push(callback);
}

void ResourceContainer::removeCallback(const Gaff::FunctionBinder<void, ResourceContainer*>& callback)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);
	auto it = _callbacks.linearSearch(callback);

	if (it != _callbacks.end()) {
		_callbacks.fastErase(it);
	}
}

void ResourceContainer::setResource(Gaff::IVirtualDestructor* resource)
{
	assert(resource && !_resource);
	AtomicCompareExchangePointer((volatile PVOID*)&_resource, resource, nullptr);
}

void ResourceContainer::callCallbacks(void)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);

	for (unsigned int i = 0; i < _callbacks.size(); ++i) {
		_callbacks[i](this);
	}

	_callbacks.clear();
}


// Resource Manager
REF_IMPL_REQ(ResourceManager);
SHIB_REF_IMPL(ResourceManager)
.addBaseClassInterfaceOnly<ResourceManager>()
;

const char* ResourceManager::GetName(void)
{
	return "Resource Manager";
}

ResourceManager::ResourceManager(void):
	_app(GetApp())
{
}

ResourceManager::~ResourceManager(void)
{
	for (auto it = _resource_cache.begin(); it != _resource_cache.end(); ++it) {
		//assert(!it.getValue()->getRefCount());
		it.getValue().set(nullptr);
	}

	_resource_cache.clear();
}

const char* ResourceManager::getName(void) const
{
	return GetName();
}

void ResourceManager::registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& resource_types, unsigned int thread_pool, const Array<JSONModifiers>& json_elements)
{
	assert(res_loader && resource_types.size());

	Gaff::SharedPtr<Array<JSONModifiers>, ProxyAllocator> je(GetAllocator()->template allocT< Array<JSONModifiers> >(json_elements));
	LoaderData loader_data = { je, ResourceLoaderPtr(res_loader), thread_pool };

	for (auto it = resource_types.begin(); it != resource_types.end(); ++it) {
		assert(_resource_loaders.indexOf(AHashString(*it)) == SIZE_T_FAIL);
		_resource_loaders[AHashString(*it)] = loader_data;
	}
}

void ResourceManager::registerResourceLoader(IResourceLoader* res_loader, const char* resource_type, unsigned int thread_pool, const Array<JSONModifiers>& json_elements)
{
	// We've already registered a loader for this file type.
	assert(_resource_loaders.indexOf(AHashString(resource_type)) == SIZE_T_FAIL);
	Gaff::SharedPtr<Array<JSONModifiers>, ProxyAllocator> je(GetAllocator()->template allocT< Array<JSONModifiers> >(json_elements));
	LoaderData loader_data = { je, ResourceLoaderPtr(res_loader), thread_pool };
	_resource_loaders[AHashString(resource_type)] = loader_data;
}

ResourcePtr ResourceManager::requestResource(const char* resource_type, const char* instance_name, uint64_t user_data)
{
	assert(resource_type && strlen(resource_type) && instance_name && strlen(instance_name));

	AHashString res_key(instance_name);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it,
		// so make a load request.
		assert(_resource_loaders.indexOf(AHashString(resource_type)) != SIZE_T_FAIL);

		ResourceContainer* res_cont = reinterpret_cast<ResourceContainer*>(GetAllocator()->alloc(sizeof(ResourceContainer)));
		new (res_cont) ResourceContainer(res_key, this, &ResourceManager::zeroRefCallback, user_data);

		ResourcePtr& res_ptr = _resource_cache[res_key];
		res_ptr.set(res_cont);

		for (auto cbs = _request_added_callbacks.begin(); cbs != _request_added_callbacks.end(); ++cbs) {
			(*cbs)(res_ptr);
		}

		// make load task
		LoaderData& loader_data = _resource_loaders[AHashString(resource_type)];

		ResourceData* res_data = GetAllocator()->template allocT<ResourceData>();
		res_data->json_elements = nullptr;
		res_data->res_loader = loader_data.res_loader;
		res_data->res_ptr = res_ptr;
		res_data->job_pool = loader_data.job_pool;

		Gaff::JobData job_data(&ResourceLoadingJob, res_data);
		_app.getJobPool().addJobs(&job_data, 1, nullptr, loader_data.job_pool);

		return res_ptr;

	} else {
		// We have a cache of this resource or we've already made a request to load it.
		// Send the container/future back.
		return it.getValue();
	}
}

ResourcePtr ResourceManager::requestResource(const char* filename, uint64_t user_data)
{
	assert(filename && strlen(filename));

	AHashString res_key(filename);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it,
		// so make a load request.
		AString extension = res_key.getString().getExtension('.');
		assert(extension.size() && _resource_loaders.indexOf(AHashString(extension)) != SIZE_T_FAIL);

		ResourceContainer* res_cont = reinterpret_cast<ResourceContainer*>(GetAllocator()->alloc(sizeof(ResourceContainer)));
		new (res_cont) ResourceContainer(res_key, this, &ResourceManager::zeroRefCallback, user_data);

		ResourcePtr& res_ptr = _resource_cache[res_key];
		res_ptr.set(res_cont);

		for (auto cbs = _request_added_callbacks.begin(); cbs != _request_added_callbacks.end(); ++cbs) {
			(*cbs)(res_ptr);
		}

		// make load task
		LoaderData& loader_data = _resource_loaders[extension];

		ResourceData* res_data = GetAllocator()->template allocT<ResourceData>();
		res_data->json_elements = loader_data.json_elements.get();
		res_data->res_loader = loader_data.res_loader;
		res_data->res_ptr = res_ptr;
		res_data->job_pool = loader_data.job_pool;

		Gaff::JobData job_data(&ResourceReadingJob, res_data);
		_app.getJobPool().addJobs(&job_data, 1, nullptr, loader_data.job_pool);

		return res_ptr;

	} else {
		// We have a cache of this resource or we've already made a request to load it.
		// Send the container/future back.
		return it.getValue();
	}
}

ResourcePtr ResourceManager::loadResourceImmediately(const char* filename, uint64_t user_data, HashMap<AString, IFile*>& file_map)
{
	AHashString res_key(filename);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it.
		AString extension = res_key.getString().getExtension('.');
		assert(extension.size() && _resource_loaders.indexOf(AHashString(extension)) != SIZE_T_FAIL);

		ResourceContainer* res_cont = reinterpret_cast<ResourceContainer*>(GetAllocator()->alloc(sizeof(ResourceContainer)));
		new (res_cont)ResourceContainer(res_key, this, &ResourceManager::zeroRefCallback, user_data);

		ResourcePtr& res_ptr = _resource_cache[res_key];
		res_ptr.set(res_cont);

		LoaderData& loader_data = _resource_loaders[extension];

		Gaff::IVirtualDestructor* data = loader_data.res_loader->load(filename, user_data, file_map);

		if (data) {
			res_ptr->setResource(data);
			res_ptr->_res_state = ResourceContainer::RS_LOADED;

		} else {
			// Log error
			res_ptr->_res_state = ResourceContainer::RS_FAILED;
		}

		return res_ptr;

	} else {
		// We have a cache of this resource. Send the container back.
		return it.getValue();
	}
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

void ResourceManager::zeroRefCallback(const AHashString& res_key)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	assert(_resource_cache.indexOf(res_key) != SIZE_T_FAIL);
	_resource_cache[res_key].set(nullptr);
	_resource_cache.erase(res_key);
}

NS_END
