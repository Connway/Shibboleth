/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_IResource.h"
#include "Shibboleth_LoadFileCallbackAttribute.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include "Shibboleth_ResourceManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Memory.h>
#include <Shibboleth_IApp.h>
#include <EASTL/algorithm.h>

SHIB_REFLECTION_DEFINE_BEGIN(ResourceState)
	.entry("Pending", ResourceState::Pending)
	.entry("Failed", ResourceState::Failed)
	.entry("Loaded", ResourceState::Loaded)
	.entry("Delayed", ResourceState::Delayed)
SHIB_REFLECTION_DEFINE_END(ResourceState)

SHIB_REFLECTION_DEFINE_BEGIN(IResource)
	.func("requestLoad", &IResource::requestLoad)
	.func("getFilePath", &IResource::getFilePath)
	.func("getState", &IResource::getState)
	.func("hasFailed", &IResource::hasFailed)
	.func("isPending", &IResource::isPending)
	.func("isLoaded", &IResource::isLoaded)
SHIB_REFLECTION_DEFINE_END(IResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(IResource)

static void LoadJob(uintptr_t thread_id_int, void* data)
{
	eastl::pair<IResource*, IFile*>* job_data = reinterpret_cast<eastl::pair<IResource*, IFile*>*>(data);

	const ILoadFileCallbackAttribute* const cb_attr = job_data->first->getReflectionDefinition().GET_CLASS_ATTR(ILoadFileCallbackAttribute);
	cb_attr->callCallback(job_data->first->getBasePointer(), job_data->second, thread_id_int);

	if (!cb_attr->doesCallbackCloseFile()) {
		GetApp().getFileSystem().closeFile(job_data->second);
	}

	SHIB_FREET(job_data, GetAllocator());
}

void IResource::requestLoad(void)
{
	if (_state != ResourceState::Delayed) {
		return;
	}

	_res_mgr->requestLoad(*this);
}

void IResource::load(void)
{
	const IFile* const file = loadFile(getFilePath().getBuffer());

	if (file) {
		const ILoadFileCallbackAttribute* const cb_attr = getReflectionDefinition().GET_CLASS_ATTR(ILoadFileCallbackAttribute);
		GAFF_ASSERT(cb_attr);

		eastl::pair<IResource*, const IFile*>* const res_data = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(eastl::pair<IResource*, const IFile*>),
			ProxyAllocator("Resource"),
			this,
			file
		);

		Gaff::JobData job_data = { LoadJob, res_data };
		GetApp().getJobPool().addJobs(&job_data, 1U, nullptr, cb_attr->getPool());

	} else {
		failed();
	}
}

void IResource::addRef(void) const
{
	++_count;
}

void IResource::release(void) const
{
	int32_t new_count = --_count;

	if (!new_count) {
		_res_mgr->removeResource(*this);
	}
}

int32_t IResource::getRefCount(void) const
{
	return _count;
}

const HashString64<>& IResource::getFilePath(void) const
{
	return _file_path;
}

ResourceState IResource::getState(void) const
{
	return _state;
}

bool IResource::hasFailed(void) const
{
	return _state == ResourceState::Failed;
}

bool IResource::isPending(void) const
{
	return _state == ResourceState::Pending;
}

bool IResource::isLoaded(void) const
{
	return _state == ResourceState::Loaded;
}

const IFile* IResource::loadFile(const char* file_path)
{
	char temp[1024] = { 0 };
	snprintf(temp, 1024, "Resources/%s", file_path);

	const IFile* file = GetApp().getFileSystem().openFile(temp);

	if (!file) {
		// $TODO: Log error.
		failed();
	}

	return file;
}

void IResource::succeeded(void)
{
	_state = ResourceState::Loaded;
}

void IResource::failed(void)
{
	_state = ResourceState::Failed;
}

NS_END
