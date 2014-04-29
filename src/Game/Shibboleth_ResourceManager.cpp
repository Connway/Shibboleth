/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

ResourceContainer::ResourceContainer(const AHashString& res_key, ResourceManager* res_manager, ZRC zero_ref_callback):
	_res_key(res_key), _zero_ref_callback(zero_ref_callback),
	_res_manager(res_manager), _resource(nullptr), _ref_count(0)
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

		Shibboleth::GetAllocator().freeT(_resource);
		Shibboleth::GetAllocator().freeT(this);
	}
}

unsigned int ResourceContainer::getRefCount(void) const
{
	return _ref_count;
}

const AHashString& ResourceContainer::getResourceKey(void) const
{
	return _res_key;
}

bool ResourceContainer::isLoaded(void) const
{
	return _resource != nullptr;
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
	Gaff::File file(_res_ptr->getResourceKey().getString().getBuffer());
	Gaff::IVirtualDestructor* data = nullptr;

	if (!file.isOpen()) {
		data = _res_loader->load(file);

		if (data) {
			_res_ptr->setResource(data);
		}
	}

	_res_ptr->callCallbacks(data != nullptr);
}



ResourceManager::ResourceManager(App& app):
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

ResourcePtr ResourceManager::requestResource(const char* filename)
{
	AHashString res_key(filename);

	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);

	if (_resource_cache.indexOf(res_key) == -1) {
		// We have no cache of this resource or have yet to make a request for it,
		// so make a load request.

		AString extension = res_key.getString().getExtension('.');
		assert(extension.size() && _resource_loaders.indexOf(AHashString(extension)) != -1);

		ResourceContainer* res_cont = (ResourceContainer*)GetAllocator().alloc(sizeof(ResourceContainer));
		new (res_cont) ResourceContainer(res_key, this, &ResourceManager::zeroRefCallback);

		ResourcePtr& res_ptr = _resource_cache[res_key];
		res_ptr.set(res_cont);

		// make load task
		ResourceLoaderPtr& res_loader = _resource_loaders[extension];

		ResourceLoadingTask* load_task = GetAllocator().template allocT<ResourceLoadingTask>(res_loader, res_ptr);
		_app.addTask(load_task);

		return res_ptr;

	} else {
		// We have a cache of this resource or we've already made a request to load it.
		// Send the container/future back.
		return _resource_cache[res_key];
	}
}

void ResourceManager::zeroRefCallback(const AHashString& res_key)
{
	assert(_resource_cache.indexOf(res_key) != -1);
	Gaff::ScopedLock<Gaff::SpinLock> lock(_res_cache_lock);
	_resource_cache[res_key].set(nullptr);
	_resource_cache.erase(res_key);
}

NS_END
