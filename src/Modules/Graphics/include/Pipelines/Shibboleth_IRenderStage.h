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

#include <Reflection/Shibboleth_Reflection.h>

NS_SHIBBOLETH

struct RenderCommandData;
class RenderManager;

class IRenderStage : public Refl::IReflectionObject
{
public:
	virtual ~IRenderStage(void) {}

	virtual bool init(RenderManager& /*render_mgr*/) { return true; }
	virtual void destroy(RenderManager& /*render_mgr*/) {}

	virtual void update(uintptr_t thread_id_int) = 0;

	virtual const RenderCommandData& getRenderCommands(void) const = 0;
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::IRenderStage)