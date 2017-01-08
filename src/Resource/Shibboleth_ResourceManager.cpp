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
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ReflectionInterfaces.h>
#include <Gaff_Assert.h>
#include <EASTL/algorithm.h>
#include <mutex>

NS_SHIBBOLETH

ResourceManager::ResourceManager(void)
{
	GetApp().registerTypeBucket(Gaff::FNV1aHash64Const("IResource"));
}

void ResourceManager::allModulesLoaded(void)
{
	IApp& app = GetApp();
	const Vector<Gaff::Hash64>* type_bucket = app.getTypeBucket(Gaff::FNV1aHash64Const("IResource"));
	GAFF_ASSERT(type_bucket);

	for (Gaff::Hash64 class_hash : *type_bucket) {
		//GAFF_ASSERT(_resource_factories.find(class_hash) == _resource_factories.end());

		const Gaff::IReflectionDefinition* ref_def = app.getReflection(class_hash);

		Gaff::Hash64 ctor_hash = Gaff::CalcTemplateHash<>(Gaff::INIT_HASH64);
		FactoryFunc factory_func = reinterpret_cast<FactoryFunc>(ref_def->getFactory(ctor_hash));

		GAFF_ASSERT_MSG(factory_func, "Class '%s' does not have a default constructor!", ref_def->getReflectionInstance().getName());

		_resource_factories[class_hash] = factory_func;
	}
}

IResourcePtr ResourceManager::requestResource(Gaff::HashStringTemp64 name)
{
	std::lock_guard<Gaff::SpinLock> lock(_res_lock);

	auto it_res = eastl::lower_bound(
		_resources.begin(),
		_resources.end(),
		name.getHash(),
		[](const IResourcePtr& lhs, Gaff::Hash64 rhs) -> bool
		{
			return lhs->getFilePath().getHash() < rhs;
		}
	);

	if (it_res != _resources.end() && (*it_res)->getFilePath().getHash() == name.getHash()) {
		return *it_res;
	}

	size_t pos = Gaff::FindLastOf(name.getBuffer(), '.');

	if (pos == SIZE_T_FAIL) {
		// Log error
		return IResourcePtr();
	}

	// Search for res_factory that handles this resource file.
	Gaff::Hash32 res_file_hash = Gaff::FNV1aHash32String(name.getBuffer() + pos);
	auto it_fact = _resource_factories.find(res_file_hash);

	if (it_fact == _resource_factories.end()) {
		// Log error
		return IResourcePtr();
	}

	// Assume all resources always inherit from IResource first.
	IResource* res = reinterpret_cast<IResource*>(it_fact->second(_allocator));
	IResourcePtr res_ptr(res);
	res->setResourceManager(this);

	_resources.insert(it_res, res_ptr);

	// Create and add resource load job.
	Gaff::JobData job_data = { ResourceManager::ResourceFileLoadJob, res };
	GetApp().getJobPool().addJobs(&job_data, 1, nullptr, (res->readsFromDisk()) ? JP_READ_FILE : 0);

	return res_ptr;
}

void ResourceManager::removeResource(const IResource* resource)
{
	std::lock_guard<Gaff::SpinLock> lock(_res_lock);

	auto it_res = eastl::lower_bound(_resources.begin(), _resources.end(), resource);

	if (it_res != _resources.end() && *it_res == resource) {
		_resources.erase(it_res);
	}
}

void ResourceManager::ResourceFileLoadJob(void* data)
{
	IResource* res = reinterpret_cast<IResource*>(data);
	res->load();
}

NS_END
