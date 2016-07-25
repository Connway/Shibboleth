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

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_HashMap.h>
#include <Shibboleth_String.h>
#include <Shibboleth_Array.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_SpinLock.h>
#include <Gaff_RefPtr.h>

NS_SHIBBOLETH

class ResourceContainerBase;
class IResourceLoader;
class IFile;

using ResourceLoaderPtr = Gaff::SharedPtr<IResourceLoader, ProxyAllocator>;
using ResourcePtr = Gaff::RefPtr<ResourceContainerBase>;

class ResourceContainerBase : public Gaff::IRefCounted
{
public:
	enum ResourceState
	{
		RS_NONE,
		RS_PENDING,
		RS_FAILED,
		RS_LOADED
	};

	template <class T>
	const T* getResourcePtr(void) const
	{
		return (T*)_resource;
	}

	template <class T>
	T* getResourcePtr(void)
	{
		return (T*)_resource;
	}

	template <class T>
	const T& getResource(void) const
	{
		return *((T*)_resource);
	}

	template <class T>
	T& getResource(void)
	{
		return *((T*)_resource);
	}

	ResourceContainerBase(const AHashString& res_key, uint64_t user_data):
		 _res_key(res_key), _resource(nullptr), _user_data(user_data),
		_ref_count(0), _res_state(RS_NONE)
	{
	}

	virtual ~ResourceContainerBase(void) {}

	void addRef(void) const { AtomicIncrement(&_ref_count); }
	virtual void release(void) const = 0;
	//void release(void) const
	//{
	//	unsigned int new_count = AtomicDecrement(&_ref_count);

	//	if (!new_count) {
	//		// Tell ResourceManager that the client no longer has references to this resource, so remove it from your list.
	//		(_res_manager->*_zero_ref_callback)(_res_key);

	//		if (_resource) {
	//			SHIB_FREET(_resource, *GetAllocator());
	//		}

	//		SHIB_FREET(this, *GetAllocator());
	//	}
	//}

	unsigned int getRefCount(void) const { return _ref_count; }
	uint64_t getUserData(void) const { return _user_data; }

	const AHashString& getResourceKey(void) const { return _res_key; }

	ResourceState getResourceState(void) const { return _res_state; }

	bool isLoaded(void) const { return _res_state == RS_LOADED; }
	bool isPending(void) const { return _res_state == RS_PENDING; }
	bool hasFailed(void) const { return _res_state == RS_FAILED; }

	void addCallback(const Gaff::FunctionBinder<void, ResourceContainerBase*>& callback)
	{
		// We're already loaded!
		if (_resource != nullptr) {
			callback(this);
			return;
		}

		Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);
		_callbacks.push(callback);
	}

	void removeCallback(const Gaff::FunctionBinder<void, ResourceContainerBase*>& callback)
	{
		Gaff::ScopedLock<Gaff::SpinLock> lock(_callback_lock);
		auto it = _callbacks.linearSearch(callback);

		if (it != _callbacks.end()) {
			_callbacks.fastErase(it);
		}
	}

protected:
	AHashString _res_key;

	Array< Gaff::FunctionBinder<void, ResourceContainerBase*> > _callbacks;

	volatile Gaff::IVirtualDestructor* _resource;

	uint64_t _user_data;

	mutable volatile unsigned int _ref_count;

	Gaff::SpinLock _callback_lock;

	ResourceState _res_state;
};

class IResourceManager
{
public:
	//using FilePopulationFunc = void (*)(void);

	struct FileReadInfo
	{
		//FilePopulationFunc file_func;
		AString json_element;
		AString append_to_filename;
		bool optional;
	};

	IResourceManager(void) {}
	virtual ~IResourceManager(void) {}

	virtual void registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& resource_types, unsigned int thread_pool = 0, const Array<FileReadInfo>& json_elements = Array<FileReadInfo>()) = 0;
	virtual void registerResourceLoader(IResourceLoader* res_loader, const char* resource_type, unsigned int thread_pool = 0, const Array<FileReadInfo>& json_elements = Array<FileReadInfo>()) = 0;

	virtual ResourcePtr requestResource(const char* resource_type, const char* instance_name, uint64_t user_data = 0) = 0;
	virtual ResourcePtr requestResource(const char* filename, uint64_t user_data = 0) = 0;

	/***********************************************************************************************************
	Blocks calling thread. Calls the loader immediately. On return, resource should be valid if succeeded.
	Should only be used by other resource loaders.

	WARNING: DO NOT MIX requestResource() AND loadResourceImmediately() CALLS! USE ONE OR THE OTHER, NOT BOTH!
	MIXING CALLS CAN POTENTIALLY CAUSE DEADLOCK!
	***********************************************************************************************************/
	virtual ResourcePtr loadResourceImmediately(const char* filename, uint64_t user_data, HashMap<AString, IFile*>& file_map) = 0;

	virtual void addRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback) = 0;
	virtual void removeRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback) = 0;

	SHIB_INTERFACE_REFLECTION(IResourceManager)
	SHIB_INTERFACE_MANAGER("Resource Manager")
};

NS_END
