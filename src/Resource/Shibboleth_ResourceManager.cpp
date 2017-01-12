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
#include "Shibboleth_ResourceExtensionAttribute.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_ReflectionInterfaces.h>
#include <Gaff_Assert.h>
#include <EASTL/algorithm.h>
#include <mutex>

SHIB_REFLECTION_DEFINE(ResourceManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ResourceManager)

ResourceManager::ResourceManager(void)
{
	GetApp().registerTypeBucket(Gaff::FNV1aHash64Const("IResource"));
}

void ResourceManager::allModulesLoaded(void)
{
	IApp& app = GetApp();
	const Vector<Gaff::Hash64>* type_bucket = app.getTypeBucket(Gaff::FNV1aHash64Const("IResource"));
	GAFF_ASSERT(type_bucket);

	Vector<const ResExtAttribute*> ext_attrs;

	for (Gaff::Hash64 class_hash : *type_bucket) {
		const Gaff::IReflectionDefinition* ref_def = app.getReflection(class_hash);
		FactoryFunc factory_func = ref_def->template getFactory<>();

		ref_def->getClassAttributes(ext_attrs);

		GAFF_ASSERT_MSG(factory_func, "Resource '%s' does not have a default constructor!", ref_def->getReflectionInstance().getName());
		GAFF_ASSERT_MSG(!ext_attrs.empty(), "Resource '%s' does not have any ResExtAttribute's!", ref_def->getReflectionInstance().getName());

		for (const ResExtAttribute * ext_attr : ext_attrs) {
			GAFF_ASSERT_MSG(
				_resource_factories.find(ext_attr->getExtension().getHash()) == _resource_factories.end(),
				"File extension '%s' already has a resource associated with it!",
				ext_attr->getExtension().getBuffer()
			);

			_resource_factories[ext_attr->getExtension().getHash()] = factory_func;
		}

		ext_attrs.clear();
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
