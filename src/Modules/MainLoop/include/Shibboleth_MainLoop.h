/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IMainLoop.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Vector.h>

NS_SHIBBOLETH

class IRenderManager;

class MainLoop : public IMainLoop
{
public:
	bool init(void) override;
	void destroy(void) override;
	void update(void) override;

private:
	struct UpdateRow final
	{
		Vector< UniquePtr<ISystem> > systems{ ProxyAllocator("MainLoop") };
		Vector<Gaff::JobData> job_data{ ProxyAllocator("MainLoop") };
	};

	struct UpdateBlock final
	{
		Vector<UpdateRow> rows{ ProxyAllocator("MainLoop") };
		Gaff::Counter counter = -1;
		int32_t curr_row = -1;
		int32_t frame = 0;

		UpdateBlock(void) = default;

		UpdateBlock(UpdateBlock&& block):
			rows(std::move(block.rows)),
			counter(static_cast<int32_t>(block.counter)),
			curr_row(block.curr_row),
			frame(block.frame)
		{
		}
	};

	Vector<UpdateBlock> _blocks{ ProxyAllocator("MainLoop") };
	IRenderManager* _render_mgr = nullptr;
	JobPool* _job_pool = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(MainLoop);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::MainLoop)
