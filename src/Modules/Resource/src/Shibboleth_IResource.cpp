/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Shibboleth_ResourceAttributesCommon.h"
#include "Shibboleth_ResourceLogging.h"
#include "Shibboleth_ResourceManager.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Memory.h>
#include <Shibboleth_IApp.h>
#include <EASTL/algorithm.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ResourceState)
	.entry("Pending", Shibboleth::ResourceState::Pending)
	.entry("Failed", Shibboleth::ResourceState::Failed)
	.entry("Loaded", Shibboleth::ResourceState::Loaded)
	.entry("Delayed", Shibboleth::ResourceState::Delayed)
SHIB_REFLECTION_DEFINE_END(Shibboleth::ResourceState)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::IResource)
	.BASE(Refl::IReflectionObject)

	.classAttrs(
		Shibboleth::ClassBucketAttribute()
	)

	.func("requestLoad", &Shibboleth::IResource::requestLoad)
	.func("getFilePath", &Shibboleth::IResource::getFilePath)
	.func("getState", &Shibboleth::IResource::getState)
	.func("hasFailed", &Shibboleth::IResource::hasFailed)
	.func("isPending", &Shibboleth::IResource::isPending)
	.func("isLoaded", &Shibboleth::IResource::isLoaded)
SHIB_REFLECTION_DEFINE_END(Shibboleth::IResource)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(IResource)

static void LoadJob(uintptr_t thread_id_int, void* data)
{
	eastl::pair<IResource*, const IFile*>* const job_data = static_cast<eastl::pair<IResource*, const IFile*>*>(data);

	job_data->first->load(*job_data->second, thread_id_int);
	GetApp().getFileSystem().closeFile(job_data->second);

	SHIB_FREET(job_data, GetAllocator());
}

void IResource::requestLoad(void)
{
	if (_state != ResourceState::Deferred) {
		return;
	}

	_res_mgr->requestLoad(*this);
}

void IResource::load(const ISerializeReader& reader, uintptr_t /*thread_id_int*/)
{
	getReflectionDefinition().load(reader, getBasePointer());
}

void IResource::load(const IFile& file, uintptr_t thread_id_int)
{
	const ResourceSchemaAttribute* const schema_attr = getReflectionDefinition().GET_CLASS_ATTR(ResourceSchemaAttribute);
	const char8_t* const schema = (schema_attr) ? schema_attr->getSchema() : nullptr;
	SerializeReaderWrapper reader_wrapper;

	if (!OpenJSONOrMPackFile(reader_wrapper, getFilePath().getBuffer(), file, false, schema)) {
		LogErrorResource("Failed to load resource '%s' with error: '%s'", getFilePath().getBuffer(), reader_wrapper.getErrorText());
		failed();
		return;
	}

	load(*reader_wrapper.getReader(), thread_id_int);
}

void IResource::load(void)
{
	const IFile* const file = loadFile(getFilePath().getBuffer());

	if (file) {
		eastl::pair<IResource*, const IFile*>* const res_data = SHIB_ALLOCT(
			GAFF_SINGLE_ARG(eastl::pair<IResource*, const IFile*>),
			ProxyAllocator("Resource"),
			this,
			file
		);

		const ResourceLoadPoolAttribute* const pool_attr = getReflectionDefinition().GET_CLASS_ATTR(ResourceLoadPoolAttribute);
		Gaff::JobData job_data = { LoadJob, res_data };

		if (pool_attr) {
			GetApp().getJobPool().addJobs(&job_data, 1, nullptr, pool_attr->getPool());
		} else {
			GetApp().getJobPool().addJobs(&job_data);
		}

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
	const int32_t new_count = --_count;

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

void IResource::succeeded(void)
{
	_state = ResourceState::Loaded;
}

void IResource::failed(void)
{
	_state = ResourceState::Failed;
}

const IFile* IResource::loadFile(const char8_t* file_path)
{
	U8String final_path(ProxyAllocator("Resource"));
	final_path.sprintf(u8"Resources/%s", file_path);

	const IFile* const file = GetApp().getFileSystem().openFile(final_path.data());

	if (!file) {
		// $TODO: Log error.
		failed();
	}

	return file;
}

NS_END
