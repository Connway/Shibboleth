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

//REF_IMPL_SHIB(CameraComponent::Viewport)
//.addFloat("X", &CameraComponent::Viewport::x)
//.addFloat("Y", &CameraComponent::Viewport::y)
//.addFloat("Width", &CameraComponent::Viewport::width)
//.addFloat("Height", &CameraComponent::Viewport::height)
//;

REF_IMPL_REQ(CameraComponent);
REF_IMPL_SHIB(CameraComponent)
.addBaseClassInterfaceOnly<CameraComponent>()
//.addArray("Viewport", &CameraComponent::_viewport)
//.addArray("Clear Color", &CameraComponent::_clear_color)
.addUInt("Render Order", &CameraComponent::_render_order)
.addFloat("Field of View", &CameraComponent::_fov)
.addBool("Active", &CameraComponent::_active)
;

CameraComponent::CameraComponent(void):
	_fov(Gaff::DegToRad * 90.0f), _active(true)
{
	_clear_color[0] = 0.0f;
	_clear_color[1] = 0.0f;
	_clear_color[2] = 0.0f;
	_clear_color[3] = 1.0f;

	_viewport[0] = 0.0f;
	_viewport[1] = 0.0f;
	_viewport[2] = 1.0f;
	_viewport[3] = 1.0f;
}

CameraComponent::~CameraComponent(void)
{
}

bool CameraComponent::load(const Gaff::JSON& json)
{
	gRefDef.read(json, this);
	// request render target resource

	return true;
}

bool CameraComponent::save(Gaff::JSON& json)
{
	gRefDef.write(json, this);

	Gaff::JSON viewport = Gaff::JSON::createArray();
	json.setObject("Viewport", viewport);

	Gaff::JSON clear_color = Gaff::JSON::createArray();
	json.setObject("Clear Color", clear_color);

	return true;
}

const float* CameraComponent::getViewport(void) const
{
	return _viewport;
}

bool CameraComponent::isActive(void) const
{
	return _active;
}

float CameraComponent::getFOV(void) const
{
	return _fov;
}

NS_END
