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

#pragma once

#include "Shibboleth_ISystem.h"
#include <Containers/Shibboleth_InstancedArray.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Error.h>

NS_SHIBBOLETH

class ISystem;

struct UpdateRow final
{
	InstancedArray<ISystem> systems{ ProxyAllocator("MainLoop") };
	Vector<Gaff::JobData> job_data{ ProxyAllocator("MainLoop") };
};

// Workaround for not supporting n-dimensional vectors.
struct UpdateBlock final
{
	Vector<UpdateRow> update_rows{ ProxyAllocator("MainLoop") };

	Gaff::Counter counter = -1;
	int32_t curr_row = -1;
	int32_t frame = 0;


	UpdateBlock(UpdateBlock&& block);
	UpdateBlock& operator=(UpdateBlock&& rhs);

	GAFF_STRUCTORS_DEFAULT(UpdateBlock);
};

class UpdatePhases final
{
public:
	Error init(void);
	void update(void);
	void clear(void);

private:
	Vector<UpdateBlock> update_blocks{ ProxyAllocator("MainLoop") };
	JobPool* _job_pool = nullptr;

	SHIB_REFLECTION_ALLOW_PRIVATE_ACCESS(UpdatePhases);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::UpdatePhases)
SHIB_REFLECTION_DECLARE(Shibboleth::UpdateBlock)
SHIB_REFLECTION_DECLARE(Shibboleth::UpdateRow)

SHIB_REFLECTION_VAR_NO_COPY(Shibboleth::UpdateBlock)
SHIB_REFLECTION_VAR_NO_COPY(Shibboleth::UpdateRow)
