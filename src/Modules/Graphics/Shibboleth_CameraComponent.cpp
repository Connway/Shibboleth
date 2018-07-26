/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_CameraComponent.h"
#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_IManager.h>

SHIB_REFLECTION_DEFINE(CameraComponent)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(CameraComponent)
	//.BASE(Component)
	.ctor<>()

	.var("display_tag", &CameraComponent::_display_tag)
	.var("fov", &CameraComponent::_fov)

	.var("start_active", &CameraComponent::_start_active)

SHIB_REFLECTION_CLASS_DEFINE_END(CameraComponent)

//void CameraComponent::onAddToWorld(void)
//{
//	_render_mgr = INTERFACE_CAST(IRenderManager, *GetApp().getManager(Gaff::FNV1aHash64Const("IRenderManager")));
//
//	if (_start_active) {
//		//_render_mgr->setActiveCamera(this);
//	}
//}

NS_END
