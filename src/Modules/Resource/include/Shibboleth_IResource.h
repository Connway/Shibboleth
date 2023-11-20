/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include <Shibboleth_Vector.h>
#include <Gaff_IncludeEASTLAtomic.h>
#include <Gaff_RefPtr.h>
#include <EASTL/functional.h>

NS_SHIBBOLETH

class ResourceManager;
//class IFile;

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

class IResource : public Refl::IReflectionObject
{
public:
	static constexpr bool Creatable = false;

	void requestLoad(void);

	virtual void load(const ISerializeReader& reader, uintptr_t thread_id_int);
	virtual void load(const IFile& file, uintptr_t thread_id_int);
	virtual void load(void);

	void addRef(void) const;
	void release(void) const;
	int32_t getRefCount(void) const;

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
	mutable eastl::atomic<int32_t> _count = 0;

	//Vector<IResource*> _incoming_references;
	//Vector<IResource*> _outgoing_references;

	ResourceState _state = ResourceState::Delayed;
	HashString64<> _file_path;

	ResourceManager* _res_mgr = nullptr;

	friend class ResourceManager;

	SHIB_REFLECTION_CLASS_DECLARE(IResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ResourceState)
SHIB_REFLECTION_DECLARE(Shibboleth::IResource)
