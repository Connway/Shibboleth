/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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
#include "Shibboleth_ResourceManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Memory.h>
#include <Shibboleth_IApp.h>
#include <EASTL/algorithm.h>

SHIB_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_INIT(Gaff::RefPtr, T)
{
	static_assert(std::is_base_of<IResource, T>::value, "Expected RefPtr<T> to be an IResource. Override reflection if you wish to use this class with reflection.");
}

SHIB_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_LOAD(Gaff::RefPtr, T)
{
	const char* const res_path = reader.readString();
	Gaff::RefPtr<T>& res_ptr = *reinterpret_cast< Gaff::RefPtr<T>* >(object);
	res_ptr = GetApp().getManagerTUnsafe<ResourceManager>().requestResource(res_path);
}

SHIB_TEMPLATE_REFLECTION_DEFINE_SERIALIZE_SAVE(Gaff::RefPtr, T)
{
	Gaff::RefPtr<T>& res_ptr = *reinterpret_cast< Gaff::RefPtr<T>* >(object);
	writer.writeString(res_ptr->getFilePath().getBuffer());
}


NS_SHIBBOLETH

void IResource::addRef(void) const
{
	++_count;
}

void IResource::release(void) const
{
	int32_t new_count = --_count;

	if (new_count == 1) {
		_res_mgr->removeResource(this);
	} else if (!new_count) {
		SHIB_FREET(this, *GetAllocator());
	}
}

int32_t IResource::getRefCount(void) const
{
	return _count;
}

void IResource::addResourceStateCallback(eastl::function<void (IResource*)>&& callback)
{
	_callbacks.emplace_back(std::move(callback));
}

void IResource::removeResourceStateCallback(const eastl::function<void (IResource*)>& callback)
{
	auto it = eastl::find(_callbacks.begin(), _callbacks.end(), callback);

	if (it != _callbacks.end()) {
		_callbacks.erase_unsorted(it);
	}
}

const HashString64& IResource::getFilePath(void) const
{
	return _file_path;
}

IResource::ResourceState IResource::getState(void) const
{
	return _state;
}

IFile* IResource::loadFile(const char* file_path)
{
	char temp[1024] = { 0 };
	snprintf(temp, 1024, "Resources/%s", file_path);

	IFile* file = GetApp().getFileSystem()->openFile(temp);

	if (!file) {
		// Log error.
		_state = RS_FAILED;
		callCallbacks();
	}

	return file;
}

void IResource::callCallbacks(void)
{
	for (auto& cb : _callbacks) {
		cb(this);
	}

	_callbacks.clear();
}

NS_END
