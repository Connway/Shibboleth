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

#pragma once

#include <Shibboleth_Reflection.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_Vector.h>
#include <Gaff_RefPtr.h>
#include <EASTL/functional.h>
#include <atomic>

#define LogErrorResource(msg, ...) LogError(k_log_channel_resource, msg, ##__VA_ARGS__)

#define RES_FAIL_MSG(cond, msg, ...) \
	if (cond) { \
		LogErrorResource(msg, ##__VA_ARGS__); \
		failed(); \
		return; \
	}

NS_SHIBBOLETH

static constexpr Gaff::Hash32 k_log_channel_resource = Gaff::FNV1aHash32Const("Resource");


class ResourceManager;
class IFile;

class IResource;
using IResourcePtr = Gaff::RefPtr<IResource>;

struct ResourceCallbackID final
{
	Gaff::Hash64 res_id = Gaff::k_init_hash64;
	int32_t cb_id = -1;
};

enum class ResourceState
{
	Pending = 0,
	Failed,
	Loaded,
	Delayed
};

class IResource : public Gaff::IRefCounted, public Refl::IReflectionObject
{
public:
	static constexpr bool Creatable = false;

	void requestLoad(void);

	virtual void load(void);

	void addRef(void) const override;
	void release(void) const override;
	int32_t getRefCount(void) const override;

	const HashString64<>& getFilePath(void) const;
	ResourceState getState(void) const;

	bool hasFailed(void) const;
	bool isPending(void) const;
	bool isLoaded(void) const;

protected:
	const IFile* loadFile(const char8_t* file_path);

	void succeeded(void);
	void failed(void);

private:
	mutable std::atomic_int32_t _count = 0;

	ResourceState _state = ResourceState::Delayed;
	HashString64<> _file_path;

	ResourceManager* _res_mgr = nullptr;

	friend class ResourceManager;

	SHIB_REFLECTION_CLASS_DECLARE(IResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ResourceState)
SHIB_REFLECTION_DECLARE(Shibboleth::IResource)
