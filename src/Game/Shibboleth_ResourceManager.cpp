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
#include <Gaff_IVirtualDestructor.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_Atomic.h>

NS_SHIBBOLETH

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

void ResourceContainer::addCallback(const Gaff::FunctionBinder<void, const AHashString&, bool> callback)
{
	// We're already loaded!
	if (_resource != nullptr) {
		callback(_res_key, true);
		return;
	}

	Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);
	_callbacks.push(callback);
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



ResourceManager::ResourceLoadingTask::ResourceLoadingTask(ResourceLoaderPtr& res_loader, ResourcePtr& res_ptr):
	_res_loader(res_loader), _res_ptr(res_ptr)
{
}

ResourceManager::ResourceLoadingTask::~ResourceLoadingTask(void)
{
}

void ResourceManager::ResourceLoadingTask::doTask(void)
{
	Gaff::IVirtualDestructor* data = _res_loader->load(_res_ptr->getResourceKey().getString().getBuffer(), _res_ptr->getUserData());

	if (data) {
		_res_ptr->setResource(data);
	} else {
		// Log error
		_res_ptr->failed();
	}

	_res_ptr->callCallbacks(data != nullptr);
}



REF_IMPL_REQ(ResourceManager);
REF_IMPL_ASSIGN_SHIB(ResourceManager)
.addBaseClassInterfaceOnly<ResourceManager>()
;

ResourceManager::ResourceManager(IApp& app):
	_app(app)
{
}

ResourceManager::~ResourceManager(void)
{
}

const char* ResourceManager::getName(void) const
{
	return "Resource Manager";
}

void ResourceManager::registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& extensions)
{
	assert(res_loader && extensions.size());
	ResourceLoaderPtr loader_ptr(res_loader);

	for (auto it = extensions.begin(); it != extensions.end(); ++it) {
		assert(_resource_loaders.indexOf(AHashString(*it)) == -1);
		_resource_loaders[AHashString(*it)] = loader_ptr;
	}
}

void ResourceManager::registerResourceLoader(IResourceLoader* res_loader, const char* extension)
{
	// We've already registered a loader for this file type.
	assert(_resource_loaders.indexOf(AHashString(extension)) == -1);
	_resource_loaders[AHashString(extension)] = res_loader;
}

ResourcePtr ResourceManager::requestResource(const char* filename, unsigned long long user_data)
{
	AHashString res_key(filename);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it,
		// so make a load request.
		AString extension = res_key.getString().getExtension('.');
		assert(extension.size() && _resource_loaders.indexOf(AHashString(extension)) != -1);

		ResourceContainer* res_cont = (ResourceContainer*)GetAllocator()->alloc(sizeof(ResourceContainer));
		new (res_cont) ResourceContainer(res_key, this, &ResourceManager::zeroRefCallback, user_data);

		ResourcePtr& res_ptr = _resource_cache[res_key];
		res_ptr.set(res_cont);

		for (auto cbs = _request_added_callbacks.begin(); cbs != _request_added_callbacks.end(); ++cbs) {
			(*cbs)(res_ptr);
		}

		// make load task
		ResourceLoaderPtr& res_loader = _resource_loaders[extension];

		ResourceLoadingTask* load_task = GetAllocator()->template allocT<ResourceLoadingTask>(res_loader, res_ptr);
		Gaff::TaskPtr<ProxyAllocator> task(load_task);
		_app.addTask(task);

		return res_ptr;

	} else {
		// We have a cache of this resource or we've already made a request to load it.
		// Send the container/future back.
		return it.getValue();
	}
}

ResourcePtr ResourceManager::loadResourceImmediately(const char* filename, unsigned long long user_data)
{
	AHashString res_key(filename);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	auto it = _resource_cache.findElementWithKey(res_key);

	if (it == _resource_cache.end()) {
		// We have no cache of this resource or have yet to make a request for it.
		AString extension = res_key.getString().getExtension('.');
		assert(extension.size() && _resource_loaders.indexOf(AHashString(extension)) != -1);

		ResourceContainer* res_cont = (ResourceContainer*)GetAllocator()->alloc(sizeof(ResourceContainer));
		new (res_cont)ResourceContainer(res_key, this, &ResourceManager::zeroRefCallback, user_data);

		ResourcePtr& res_ptr = _resource_cache[res_key];
		res_ptr.set(res_cont);

		ResourceLoaderPtr& res_loader = _resource_loaders[extension];

		Gaff::IVirtualDestructor* data = res_loader->load(filename, user_data);

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
	assert(_resource_cache.indexOf(res_key) != -1);
	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	_resource_cache[res_key].set(nullptr);
	_resource_cache.erase(res_key);
}

NS_END
