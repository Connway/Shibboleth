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

#pragma once

#include "Shibboleth_IResourceLoader.h"
#include "Shibboleth_Array.h"
#include "Shibboleth_ITask.h"
#include "Shibboleth_App.h"
#include <Gaff_SharedPtr.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Function.h>
#include <Gaff_RefPtr.h>

NS_GAFF
	class IVirtualDestructor;
NS_END

NS_SHIBBOLETH

class ResourceManager;

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

	void addCallback(const Gaff::FunctionBinder<void, const AHashString&, bool> callback);

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

	ResourceContainer(const AHashString& res_key, ResourceManager* res_manager, ZRC zero_ref_callback, unsigned long long user_data);
	void setResource(Gaff::IVirtualDestructor* resource);
	void callCallbacks(bool succeeded);

	friend class ResourceManager;
};


typedef Gaff::SharedPtr<IResourceLoader, ProxyAllocator> ResourceLoaderPtr;
typedef Gaff::RefPtr<ResourceContainer> ResourcePtr;


class ResourceManager : public Gaff::INamedObject
{
public:
	ResourceManager(App& app);
	~ResourceManager(void);

	const char* getName(void) const;

	void registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& extensions);
	INLINE void registerResourceLoader(IResourceLoader* res_loader, const char* extension);

	ResourcePtr requestResource(const char* filename, unsigned long long user_data = 0);

private:
	class ResourceLoadingTask : public ITask
	{
	public:
		ResourceLoadingTask(ResourceLoaderPtr& res_loader, ResourcePtr& res_ptr);
		~ResourceLoadingTask(void);

		void doTask(void);

	private:
		ResourceLoaderPtr& _res_loader;
		ResourcePtr& _res_ptr;
	};

	HashMap<AHashString, ResourceLoaderPtr> _resource_loaders;
	HashMap<AHashString, ResourcePtr> _resource_cache;
	App& _app;

	Gaff::SpinLock _res_cache_lock;

	void zeroRefCallback(const AHashString& res_key);

	GAFF_NO_COPY(ResourceManager);
	GAFF_NO_MOVE(ResourceManager);
};

NS_END