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

#include "Shibboleth_IResourceManager.h"
#include <Shibboleth_IResourceLoader.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_HashMap.h>
#include <Gaff_Function.h>

NS_GAFF
	class IVirtualDestructor;
NS_END

NS_SHIBBOLETH

//class ResourceManager;
//class IApp;
//
//class ResourceContainer : public ResourceContainerBase
//{
//public:
//	~ResourceContainer(void);
//	void release(void) const override;
//
//private:
//	using ZRC = void (ResourceManager::*)(const AHashString&);
//	ResourceManager* _res_manager;
//	ZRC _zero_ref_callback;
//
//	ResourceContainer(const AHashString& res_key, ResourceManager* res_manager, ZRC zero_ref_callback, uint64_t user_data);
//	void setResource(Gaff::IVirtualDestructor* resource);
//	void callCallbacks(void);
//
//	friend void ResourceReadingJob(void*);
//	friend void ResourceLoadingJob(void*);
//	friend class ResourceManager;
//};
//
//
//class ResourceManager : public IManager, public IResourceManager
//{
//public:
//	ResourceManager(void);
//	~ResourceManager(void);
//
//	const char* getName(void) const override;
//
//	void registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& resource_types, unsigned int thread_pool = 0, const Array<FileReadInfo>& json_elements = Array<FileReadInfo>()) override;
//	void registerResourceLoader(IResourceLoader* res_loader, const char* resource_type, unsigned int thread_pool = 0, const Array<FileReadInfo>& json_elements = Array<FileReadInfo>()) override;
//
//	ResourcePtr requestResource(const char* resource_type, const char* instance_name, uint64_t user_data = 0) override;
//	ResourcePtr requestResource(const char* filename, uint64_t user_data = 0) override;
//
//	/***********************************************************************************************************
//		Blocks calling thread. Calls the loader immediately. On return, resource should be valid if succeeded.
//		Should only be used by other resource loaders.
//
//		WARNING: DO NOT MIX requestResource() AND loadResourceImmediately() CALLS! USE ONE OR THE OTHER, NOT BOTH!
//		MIXING CALLS CAN POTENTIALLY CAUSE DEADLOCK!
//	***********************************************************************************************************/
//	ResourcePtr loadResourceImmediately(const char* filename, uint64_t user_data, HashMap<AString, IFile*>& file_map) override;
//
//	void addRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback) override;
//	void removeRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback) override;
//
//private:
//	struct LoaderData
//	{
//		Gaff::SharedPtr<Array<FileReadInfo>, ProxyAllocator> json_elements;
//		ResourceLoaderPtr res_loader;
//		unsigned int job_pool;
//	};
//
//	HashMap<AHashString, LoaderData> _resource_loaders;
//	HashMap<AHashString, ResourcePtr> _resource_cache;
//	Array< Gaff::FunctionBinder<void, ResourcePtr&> > _request_added_callbacks;
//	IApp& _app;
//
//	Gaff::SpinLock _res_cache_lock;
//
//	void zeroRefCallback(const AHashString& res_key);
//
//	GAFF_NO_COPY(ResourceManager);
//	GAFF_NO_MOVE(ResourceManager);
//
//	SHIB_REF_DEF(ResourceManager);
//	REF_DEF_REQ;
//};

class ResourceManager : public IManager, public IResourceManager
{
public:
	ResourceManager(void);
	~ResourceManager(void);

	const char* getName(void) const override;

	void registerResourceLoader(IResourceLoader* res_loader, const Array<AString>& resource_types, unsigned int thread_pool = 0) override;
	void registerResourceLoader(IResourceLoader* res_loader, const char* resource_type, unsigned int thread_pool = 0) override;

	ResourcePtr requestResource(const char* resource_type, const char* instance_name, uint64_t user_data = 0) override;
	ResourcePtr requestResource(const char* filename, uint64_t user_data = 0) override;

	void addRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback) override;
	void removeRequestAddedCallback(const Gaff::FunctionBinder<void, ResourcePtr&>& callback) override;

private:
	struct LoaderData
	{
		ResourceLoaderPtr res_loader;
		unsigned int job_pool;
	};

	Gaff::SpinLock _res_cache_lock;
	HashMap<AHashString, LoaderData> _resource_loaders;
	HashMap<AHashString, ResourceContainer*> _resource_cache;
	Array< Gaff::FunctionBinder<void, ResourcePtr&> > _request_added_callbacks;

	void handleZeroRef(const AHashString& res_key) override;

	GAFF_NO_COPY(ResourceManager);
	GAFF_NO_MOVE(ResourceManager);

	SHIB_REF_DEF(ResourceManager);
	REF_DEF_REQ;
};

NS_END
