/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#define SHIB_REFL_IMPL
#include "Shibboleth_IResource.h"
#include "Shibboleth_ResourceAttributesCommon.h"
#include "Shibboleth_ResourceLogging.h"
#include "Shibboleth_ResourceManager.h"
#include <FileSystem/Shibboleth_IFileSystem.h>
#include <Shibboleth_SerializeReaderWrapper.h>
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
	.entry("Deferred", Shibboleth::ResourceState::Deferred)
SHIB_REFLECTION_DEFINE_END(Shibboleth::ResourceState)

SHIB_REFLECTION_IMPL(Shibboleth::IResource)

NS_SHIBBOLETH

void IResourceTracker::addIncomingReference(IResourceTracker& tracker)
{
	EA::Thread::AutoSpinLock lock(_incoming_references_lock);

	if (Gaff::PushBackUnique(_incoming_references, &tracker)) {
		++tracker._dependency_count;
	}
}

const Vector<IResourceTracker*>& IResourceTracker::getIncomingReferences(void) const
{
	return _incoming_references;
}

bool IResourceTracker::haveDependenciesSucceeded(void) const
{
	return !_dependency_success;
}

bool IResourceTracker::hasDependenciesFailed(void) const
{
	return !_dependency_success;
}

bool IResourceTracker::areDependenciesLoaded(void) const
{
	return _dependency_count <= 0;
}

void IResourceTracker::dependenciesLoaded(void)
{
	finishedLoading(true);
}

void IResourceTracker::finishedLoading(bool success)
{
	for (IResourceTracker* incoming_reference : getIncomingReferences()) {
		const int32_t count = --incoming_reference->_dependency_count;

		if (!success) {
			incoming_reference->_dependency_success = false;
		}

		if (count <= 0) {
			incoming_reference->dependenciesLoaded();
		}
	}
}



SHIB_REFLECTION_CLASS_DEFINE(IResource)

static void LoadJob(uintptr_t thread_id_int, void* data)
{
	eastl::pair<IResource*, const IFile*>* const job_data = static_cast<eastl::pair<IResource*, const IFile*>*>(data);

	job_data->first->load(*job_data->second, thread_id_int);
	GetApp().getFileSystem().closeFile(job_data->second);

	SHIB_FREET(job_data, GetAllocator());
}

bool IResource::waitUntilLoaded(void) const
{
	_res_mgr->waitForResource(*this);
	return getState() == ResourceState::Loaded;
}

void IResource::requestUnload(void)
{
	const int32_t load_count = --_load_count;
	GAFF_ASSERT(load_count >= 0);

	if (load_count <= 0) {
		_res_mgr->requestUnload(*this);
	}
}

void IResource::requestLoad(void)
{
	++_load_count;

	if (_state == ResourceState::Deferred) {
		_res_mgr->requestLoad(*this);
	}
}

void IResource::load(const ISerializeReader& reader, uintptr_t thread_id_int)
{
	const auto& ref_def = getReflectionDefinition();

	if (!ref_def.load(reader, getBasePointer())) {
		failed();
		return;
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	while (!areDependenciesLoaded()) {
		GetApp().getJobPool().help(thread_id);
	}

	dependenciesLoaded(reader, thread_id_int);
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
	++_ref_count;
}

void IResource::release(void) const
{
	const int32_t new_count = --_ref_count;

	if (!new_count) {
		_res_mgr->removeResource(*this);
	}
}

int32_t IResource::getRefCount(void) const
{
	return _ref_count;
}

const HashString64<>& IResource::getFilePath(void) const
{
	return _file_path;
}

ResourceState IResource::getState(void) const
{
	return _state;
}

bool IResource::isDeferred(void) const
{
	return _state == ResourceState::Deferred;
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

void IResource::dependenciesLoaded(const ISerializeReader& /*reader*/, uintptr_t /*thread_id_int*/)
{
	dependenciesLoaded();
}

void IResource::dependenciesLoaded(void)
{
	// Derived class handled setting our state already.
	if (!isPending()) {
		return;
	}

	if (haveDependenciesSucceeded()) {
		succeeded();
	} else {
		failed();
	}
}

void IResource::succeeded(void)
{
	_state = ResourceState::Loaded;
	finishedLoading(true);
}

void IResource::failed(void)
{
	_state = ResourceState::Failed;
	finishedLoading(false);
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
