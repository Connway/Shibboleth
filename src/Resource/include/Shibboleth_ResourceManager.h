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

#include "Shibboleth_IResource.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_IManager.h>
#include <Gaff_SpinLock.h>
#include <Gaff_RefPtr.h>

NS_SHIBBOLETH

class ResourceManager : public IManager
{
public:
	ResourceManager(void);

	void allModulesLoaded(void) override;

	template <size_t size>
	IResourcePtr requestResource(const char (&string)[size])
	{
		return requestResource(Gaff::HashStringTemp64(string));
	}

	IResourcePtr requestResource(const char* name)
	{
		return requestResource(Gaff::HashStringTemp64(name, eastl::CharStrlen(name)));
	}

	IResourcePtr requestResource(Gaff::HashStringTemp64 name);

private:
	using FactoryFunc = void* (*)(Gaff::IAllocator&);

	Gaff::SpinLock _res_lock;
	Vector<IResourcePtr> _resources;
	VectorMap<Gaff::Hash32, FactoryFunc> _resource_factories;
	ProxyAllocator _allocator = ProxyAllocator("Resource");

	void removeResource(const IResource* resource);

	static void ResourceFileLoadJob(void* data);

	friend class IResource;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceManager);
};

NS_END

SHIB_REFLECTION_DECLARE(ResourceManager)