#pragma once

#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>
#include <EAThread/eathread.h>

NS_SHIBBOLETH

class ThreadAllocator final : public EA::Thread::Allocator
{
public:
	void* Alloc(size_t size, const char* name = nullptr, unsigned int flags = 0) override
	{
		GAFF_REF(name, flags);
		return SHIB_ALLOC(size, GetAllocator());
	}

	void* Alloc(size_t size, const char* name, unsigned int flags, unsigned int align, unsigned int alignOffset = 0) override
	{
		GAFF_REF(name, flags, alignOffset);
		return SHIB_ALLOC_ALIGNED(size, static_cast<size_t>(align), GetAllocator());
	}

	void Free(void* block, size_t size = 0) override
	{
		GAFF_REF(size);
		SHIB_FREE(block, GetAllocator());
	}
};

NS_END
