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

#include "Shibboleth_DebugSystem.h"
#include "Shibboleth_DebugManager.h"
#include <Gaff_IncludeOptick.h>

SHIB_REFLECTION_DEFINE_BEGIN(DebugRenderSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugRenderSystem)

SHIB_REFLECTION_DEFINE_BEGIN(DebugSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugSystem)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugRenderSystem)
SHIB_REFLECTION_CLASS_DEFINE(DebugSystem)


bool DebugRenderSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugRenderSystem::update(uintptr_t thread_id_int)
{
	_debug_mgr->render(thread_id_int);
}


bool DebugSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugSystem::update(uintptr_t /*thread_id_int*/)
{
	OPTICK_EVENT();
	_debug_mgr->update();
}

NS_END
