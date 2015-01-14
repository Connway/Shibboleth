/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
REF_IMPL_SHIB(CameraComponent)
.addBaseClassInterfaceOnly<CameraComponent>()
.addString("Render Target Name", &CameraComponent::_render_target_name)
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

	//RenderManager& render_mgr = _app.getManagerT<RenderManager>("Render Manager");

	//unsigned int rt_index = render_mgr.createRT(800, 600, Gleam::ITexture::RGBA_8_UNORM, _render_target_name);

	//if (rt_index == UINT_FAIL) {
	//	// log error
	//	return false;
	//}

	// cache render target

	return true;
}

bool CameraComponent::save(Gaff::JSON& json)
{
	g_Ref_Def.write(json, this);

	Gaff::JSON display_tags = Gaff::JSON::createArray();
	unsigned int array_size = 0;

	for (unsigned int i = 0; i < g_DisplayTags_Ref_Def.getNumEntries(); ++i) {
		auto entry = g_DisplayTags_Ref_Def.getEntry(i);

		if (entry.second & _display_tags) {
			display_tags.setObject(array_size, Gaff::JSON::createString(entry.first.getBuffer()));
			++array_size;
		}
	}

	json.setObject("display_tags", display_tags);

	return true;
}

const AString& CameraComponent::getRenderTargetName(void) const
{
	return _render_target_name;
}

NS_END
