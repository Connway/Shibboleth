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

#include <Shibboleth_ECSQuery.h>
#include <Shibboleth_ISystem.h>
#include <Gleam_ICommandList.h>

NS_SHIBBOLETH

class RenderManagerBase;
class ECSManager;

class ClearRenderTargetSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	RenderManagerBase* _render_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;
	ECSQuery::Output _camera;
	int32_t _cache_index = 0;

	UniquePtr<Gleam::ICommandList> _cmd_lists[2];

	SHIB_REFLECTION_CLASS_DECLARE(ClearRenderTargetSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ClearRenderTargetSystem)
