/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_IApp.h>

#define RT_DISPLAY_TAG 1

NS_SHIBBOLETH

REF_IMPL_REQ(CameraComponent);
REF_IMPL_ASSIGN_SHIB(CameraComponent)
.addBaseClassInterfaceOnly<CameraComponent>()
.addString("Render Target Name", &CameraComponent::_render_target_name)
.addBool("Create for displays with tag?", &CameraComponent::GetFlag<RT_DISPLAY_TAG>, &CameraComponent::SetFlag<RT_DISPLAY_TAG>)
;

CameraComponent::CameraComponent(IApp& app):
	_app(app)
{
}

CameraComponent::~CameraComponent(void)
{
}

bool CameraComponent::load(const Gaff::JSON& json)
{
	g_Ref_Def.read(json, this);

	RenderManager& render_mgr = _app.getManagerT<RenderManager>("Render Manager");

	//unsigned int rt_index = render_mgr.createRT(800, 600, Gleam::ITexture::RGBA_8_UNORM, _render_target_name);

	//if (rt_index == UINT_FAIL) {
	//	// log error
	//	return false;
	//}

	// cache render target

	return true;
}

const AString& CameraComponent::getRenderTargetName(void) const
{
	return _render_target_name;
}

template <unsigned int bit>
bool CameraComponent::GetFlag(void) const
{
	return (_option_flags & (1 << bit)) != 0;
}

template <unsigned int bit>
void CameraComponent::SetFlag(bool value)
{
	if (value) {
		_option_flags |= (1 << bit);
	}
	else {
		_option_flags &= ~(1 << bit);
	}
}

NS_END
