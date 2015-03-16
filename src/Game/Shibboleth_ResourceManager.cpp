/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include <Shibboleth_IApp.h>
#include <Gaff_IVirtualDestructor.h>
#include <Gaff_ScopedExit.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_SharedPtr.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

// Resource Container
ResourceContainer::ResourceContainer(const AHashString& res_key, ResourceManager* res_manager, ZRC zero_ref_callback, unsigned long long user_data):
	_res_manager(res_manager), _zero_ref_callback(zero_ref_callback), _res_key(res_key),
	_resource(nullptr), _user_data(user_data), _ref_count(0), _failed(false)
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

unsigned long long ResourceContainer::getUserData(void) const
{
	return _user_data;
}

const AHashString& ResourceContainer::getResourceKey(void) const
{
	return _res_key;
}

bool ResourceContainer::isLoaded(void) const
{
	return _resource != nullptr;
}

bool ResourceContainer::hasFailed(void) const
{
	return _failed;
}

void ResourceContainer::failed(void)
{
	_failed = true;
}

void ResourceContainer::addCallback(const Gaff::FunctionBinder<void, const AHashString&, bool>& callback)
{
	// We're already loaded!
	if (_resource != nullptr) {
		callback(_res_key, true);
		return;
	}

	Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);
	_callbacks.push(callback);
}

void ResourceContainer::removeCallback(const Gaff::FunctionBinder<void, const AHashString&, bool>& callback)
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

void ResourceContainer::callCallbacks(bool succeeded)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);

	for (unsigned int i = 0; i < _callbacks.size(); ++i) {
		_callbacks[i](_res_key, succeeded);
	}

	_callbacks.clear();
}


// Resource Reading Task
ResourceManager::ResourceReadingTask::ResourceReadingTask(ResourceLoaderPtr& res_loader, ResourcePtr& res_ptr, const Array<JSONModifiers>& json_elements, unsigned int thread_pool):
	_json_elements(json_elements), _res_loader(res_loader), _res_ptr(res_ptr), _thread_pool(thread_pool)
{
}

ResourceManager::ResourceReadingTask::~ResourceReadingTask(void)
{
}

void ResourceManager::ResourceReadingTask::doTask(void)
{
	IFileSystem* fs = GetApp().getFileSystem();
	IFile* file = fs->openFile(_res_ptr->getResourceKey().getString().getBuffer());
	HashMap<AString, IFile*> file_map;
	bool failed = false;

	GAFF_SCOPE_EXIT([&](void)
	{
		if (failed) {
			for (auto it = file_map.begin(); it != file_map.end(); ++it) {
				GetApp().getFileSystem()->closeFile(*it);
			}

			_res_ptr->failed();
			_res_ptr->callCallbacks(false);
		}
	});

	if (file) {
		file_map[_res_ptr->getResourceKey().getString()] = file;

		// If we have specified JSON elements that reference file names, then the file we just loaded
		// is a JSON file. Parse it and load the other files referenced in it.
		if (!_json_elements.empty()) {
			Gaff::JSON json;

			if (!json.parse(file->getBuffer())) {
				failed = true;
				return;
			}

			for (auto it = _json_elements.begin(); it != _json_elements.end(); ++it) {
				Gaff::JSON file_name = json[it->json_element];

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

		// Create ResourceLoadingTask
		ResourceLoadingTask* load_task = GetAllocator()->template allocT<ResourceLoadingTask>(_res_loader, _res_ptr, file_map);
		GetApp().addTask(load_task, _thread_pool);
	}
}



// Resource Loading Task
ResourceManager::ResourceLoadingTask::ResourceLoadingTask(ResourceLoaderPtr& res_loader, ResourcePtr& res_ptr, HashMap<AString, IFile*>& file_map):
	_file_map(Gaff::Move(file_map)), _res_loader(res_loader), _res_ptr(res_ptr)
{
}

ResourceManager::ResourceLoadingTask::~ResourceLoadingTask(void)
{
}

void ResourceManager::ResourceLoadingTask::doTask(void)
{
	Gaff::IVirtualDestructor* data = _res_loader->load(_res_ptr->getResourceKey().getString().getBuffer(), _res_ptr->getUserData(), _file_map);

	if (data) {
		_res_ptr->setResource(data);
	} else {
		// Log error
		_res_ptr->failed();
	}

	_res_ptr->callCallbacks(data != nullptr);
}



// Resource Manager
REF_IMPL_REQ(ResourceManager);
REF_IMPL_SHIB(ResourceManager)
.addBaseClassInterfaceOnly<ResourceManager>()
;

ResourceManager::ResourceManager(IApp& app):
	_app(app)
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
	return "Resource Manager";
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

ResourcePtr ResourceManager::requestResource(const char* resource_type, const char* instance_name, unsigned long long user_data)
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

		ResourceLoadingTask* load_task = GetAllocator()->template allocT<ResourceLoadingTask>(loader_data.res_loader, res_ptr, HashMap<AString, IFile*>());
		_app.addTask(load_task, loader_data.thread_pool);

		return res_ptr;

	} else {
		// We have a cache of this resource or we've already made a request to load it.
		// Send the container/future back.
		return it.getValue();
	}
}

ResourcePtr ResourceManager::requestResource(const char* filename, unsigned long long user_data)
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

		ResourceReadingTask* load_task = GetAllocator()->template allocT<ResourceReadingTask>(loader_data.res_loader, res_ptr, *loader_data.json_elements, loader_data.thread_pool);
		_app.addTask(load_task, TPT_IO);

		return res_ptr;

	} else {
		// We have a cache of this resource or we've already made a request to load it.
		// Send the container/future back.
		return it.getValue();
	}
}

ResourcePtr ResourceManager::loadResourceImmediately(const char* filename, unsigned long long user_data, HashMap<AString, IFile*>& file_map)
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
		} else {
			// Log error
			res_ptr->failed();
		}

		return res_ptr;

	} else {
		// We have a cache of this resource. Send the container back.
		return it.getValue();
	}
}

void ResourceManager::addRequestAddedCallback(const Gaff::Function<void, ResourcePtr&>& callback)
{
	_request_added_callbacks.push(callback);
}

void ResourceManager::zeroRefCallback(const AHashString& res_key)
{
	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	assert(_resource_cache.indexOf(res_key) != SIZE_T_FAIL);
	_resource_cache[res_key].set(nullptr);
	_resource_cache.erase(res_key);
}

NS_END
