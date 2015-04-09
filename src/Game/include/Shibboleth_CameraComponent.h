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

#pragma once

#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceWrapper.h"
#include "Shibboleth_IComponent.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_Array.h>
#include <Gleam_Frustum_CPU.h>

NS_SHIBBOLETH

class IApp;

class CameraComponent : public IComponent
{
public:
	CameraComponent(void);
	~CameraComponent(void);

	bool validate(Gaff::JSON& json);
	bool load(const Gaff::JSON& json);
	bool save(Gaff::JSON& json);

	void* rawRequestInterface(unsigned int class_id) const;

	void updateFrustum(Object* object, unsigned long long);

	const Array<unsigned int>& getDevices(void) const;
	const Gleam::FrustumCPU& getFrustum(void) const;
	unsigned short getWindowTags(void) const;
	unsigned char getRenderOrder(void) const;
	const float* getViewport(void) const;
	void setActive(bool active);
	bool isActive(void) const;
	float getFOV(void) const;

private:
	Gleam::FrustumCPU _unstransformed_frustum;
	Gleam::FrustumCPU _frustum;

	Array<unsigned int> _devices;

	ResourceWrapper<RenderTargetData> _render_target;

	float _clear_color[4];
	float _viewport[4];
	float _fov;
	float _z_near;
	float _z_far;
	unsigned short _window_tags;
	unsigned char _render_order;
	bool _active;

	void RenderTargetCallback(const AHashString& /*resource*/, bool success);

	REF_DEF_SHIB(CameraComponent);
};

NS_END
