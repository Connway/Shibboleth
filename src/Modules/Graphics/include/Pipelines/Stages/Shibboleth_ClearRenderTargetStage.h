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

#include "Pipelines/Shibboleth_IRenderPipelineStage.h"
#include "Shibboleth_RenderCommands.h"

namespace EA::Thread
{
	class Mutex;
}

NS_SHIBBOLETH

class IResource;

class ClearRenderTargetStage final : public IRenderPipelineStage
{
public:
	bool init(RenderManager& render_mgr) override;
	//void destroy(RenderManager& /*render_mgr*/) override;

	void update(uintptr_t thread_id_int) override;

	const RenderCommandData* getRenderCommands(void) const override;

private:
	RenderCommandData _render_commands;
	RenderManager* _render_mgr = nullptr;

	const Vector<IResource*>* _render_targets = nullptr;
	EA::Thread::Mutex* _resource_lock = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(ClearRenderTargetStage);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ClearRenderTargetStage)
