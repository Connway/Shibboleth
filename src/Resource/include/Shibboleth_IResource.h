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

#include <Shibboleth_HashString.h>
#include <Shibboleth_Vector.h>
#include <Gaff_IRefCounted.h>
#include <Gaff_RefPtr.h>
#include <EASTL/functional.h>
#include <atomic>

#define RES_FAIL_MSG(cond, msg, ...) \
	if (cond) { \
		LogError(LOG_CHANNEL_RESOURCE, msg, __VA_ARGS__); \
		_state = RS_FAILED; \
		callCallbacks(); \
		return; \
	}


NS_SHIBBOLETH

class ResourceManager;
class IFile;

class IResource;
using IResourcePtr = Gaff::RefPtr<IResource>;

class IResource : public Gaff::IRefCounted
{
public:
	enum ResourceState
	{
		RS_PENDING = 0,
		RS_FAILED,
		RS_LOADED
	};

	using ResStateCallbackFunc = void (IResource*);

	virtual void load(void) = 0;
	virtual bool readsFromDisk(void) const { return true; }

	void addRef(void) const override;
	void release(void) const override;
	int32_t getRefCount(void) const override;

	void addResourceStateCallback(eastl::function<ResStateCallbackFunc>&& callback);
	void removeResourceStateCallback(const eastl::function<ResStateCallbackFunc>& callback);

	const HashString64& getFilePath(void) const;
	ResourceState getState(void) const;

protected:
	Vector<IResourcePtr> _sub_resources = Vector<IResourcePtr>(ProxyAllocator("Resource"));
	Vector< eastl::function<ResStateCallbackFunc> > _callbacks;
	ResourceState _state = RS_PENDING;

	IFile* loadFile(const char* file_path);
	void callCallbacks(void);

private:
	mutable std::atomic_int32_t _count = 0;
	HashString64 _file_path;
	ResourceManager* _res_mgr = nullptr;

	void setResourceManager(ResourceManager* res_mgr);

	friend class ResourceManager;
};

NS_END
