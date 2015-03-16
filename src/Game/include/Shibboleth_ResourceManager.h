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

#pragma once

#include "Shibboleth_IResourceLoader.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_HashMap.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_ITask.h>
#include <Gaff_SharedPtr.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Function.h>
#include <Gaff_RefPtr.h>

NS_GAFF
	class IVirtualDestructor;
NS_END

NS_SHIBBOLETH

class ResourceManager;
class IApp;

class ResourceContainer : public Gaff::IRefCounted
{
public:
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

	~ResourceContainer(void);

	INLINE void addRef(void) const;
	void release(void) const;

	INLINE unsigned int getRefCount(void) const;
	INLINE unsigned long long getUserData(void) const;

	INLINE const AHashString& getResourceKey(void) const;
	INLINE bool isLoaded(void) const;

	INLINE bool hasFailed(void) const;
	INLINE void failed(void);

	void addCallback(const Gaff::FunctionBinder<void, const AHashString&, bool>& callback);
	void removeCallback(const Gaff::FunctionBinder<void, const AHashString&, bool>& callback);

private:
	typedef void (ResourceManager::*ZRC)(const AHashString&);
	ResourceManager* _res_manager;
	ZRC _zero_ref_callback;

	AHashString _res_key;

	Array< Gaff::FunctionBinder<void, const AHashString&, bool> > _callbacks;

	volatile Gaff::IVirtualDestructor* _resource;

	unsigned long long _user_data;

	mutable volatile unsigned int _ref_count;

	Gaff::SpinLock _callback_lock;

	bool _failed;

	ResourceContainer(const AHashString& res_key, ResourceManager* res_manager, ZRC zero_ref_callback, unsigned long long user_data);
	void setResource(Gaff::IVirtualDestructor* resource);
	void callCallbacks(bool succeeded);

	friend class ResourceManager;
};


typedef Gaff::SharedPtr<IResourceLoader, ProxyAllocator> ResourceLoaderPtr;
typedef Gaff::RefPtr<ResourceContainer> ResourcePtr;


class ResourceManager : public IManager
{
public:
	struct JSONModifiers
	{
		AString json_element;
		AString append_to_filename;
		bool optional;
	};

	ResourceManager(IApp& app);
	~ResourceManager(void);

	const char* getName(void) const;

	void registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& resource_types, unsigned int thread_pool = 0, const Array<JSONModifiers>& json_elements = Array<JSONModifiers>());
	INLINE void registerResourceLoader(IResourceLoader* res_loader, const char* resource_type, unsigned int thread_pool = 0, const Array<JSONModifiers>& json_elements = Array<JSONModifiers>());

	ResourcePtr requestResource(const char* resource_type, const char* instance_name, unsigned long long user_data = 0);
	ResourcePtr requestResource(const char* filename, unsigned long long user_data = 0);

	/***********************************************************************************************************
		Blocks calling thread. Calls the loader immediately. On return, resource should be valid if succeeded.
		Should only be used by other resource loaders.

		WARNING: DO NOT MIX requestResource() AND loadResourceImmediately() CALLS! USE ONE OR THE OTHER, NOT BOTH!
		MIXING CALLS CAN POTENTIALLY CAUSE DEADLOCK!
	***********************************************************************************************************/
	ResourcePtr loadResourceImmediately(const char* filename, unsigned long long user_data, HashMap<AString, IFile*>& file_map);

	void addRequestAddedCallback(const Gaff::Function<void, ResourcePtr&>& callback);

	void* rawRequestInterface(unsigned int class_id) const;

private:
	class ResourceReadingTask : public ITask
	{
	public:
		ResourceReadingTask(ResourceLoaderPtr& res_loader, ResourcePtr& res_ptr, const Array<JSONModifiers>& json_elements, unsigned int thread_pool);
		~ResourceReadingTask(void);

		void doTask(void);

	private:
		const Array<JSONModifiers>& _json_elements; // Lists JSON elements that specify files to read.
		ResourceLoaderPtr _res_loader;
		ResourcePtr _res_ptr;
		unsigned int _thread_pool;

		SHIB_REF_COUNTED(ResourceReadingTask);
	};

	class ResourceLoadingTask : public ITask
	{
	public:
		ResourceLoadingTask(ResourceLoaderPtr& res_loader, ResourcePtr& res_ptr, HashMap<AString, IFile*>& file_map);
		~ResourceLoadingTask(void);

		void doTask(void);

	private:
		HashMap<AString, IFile*> _file_map;
		ResourceLoaderPtr _res_loader;
		ResourcePtr _res_ptr;

		SHIB_REF_COUNTED(ResourceLoadingTask);
	};

	struct LoaderData
	{
		Gaff::SharedPtr<Array<JSONModifiers>, ProxyAllocator> json_elements;
		ResourceLoaderPtr res_loader;
		unsigned int thread_pool;
	};

	HashMap<AHashString, LoaderData> _resource_loaders;
	HashMap<AHashString, ResourcePtr> _resource_cache;
	Array< Gaff::Function<void, ResourcePtr&> > _request_added_callbacks;
	IApp& _app;

	Gaff::SpinLock _res_cache_lock;

	void zeroRefCallback(const AHashString& res_key);

	GAFF_NO_COPY(ResourceManager);
	GAFF_NO_MOVE(ResourceManager);

	REF_DEF_SHIB(ResourceManager);
};

NS_END
