/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_ResourceDefines.h>
#include <Shibboleth_ResourceWrapper.h>
#include <Shibboleth_Array.h>

namespace Gleam
{
	class Matrix4x4CPU;
	class FrustumCPU;
}

NS_SHIBBOLETH

class ICameraComponent
{
public:
	enum ClearMode
	{
		CM_SKYBOX = 0,
		CM_COLOR,
		CM_DEPTH_STENCIL,
		CM_STENCIL,
		CM_NOTHING
	};

	ICameraComponent(void) {}
	virtual ~ICameraComponent(void) {}

	virtual const Gleam::Matrix4x4CPU& getProjectionMatrix(void) const = 0;
	virtual const Gleam::FrustumCPU& getFrustum(void) const = 0;
	virtual const float* getViewport(void) const = 0;
	virtual float getAspectRatio(void) const = 0;
	virtual float getFOV(void) const = 0;
	virtual void setFOV(float fov) = 0;
	virtual float getZNear(void) const = 0;
	virtual void setZNear(float z_near) = 0;
	virtual float getZFar(void) const = 0;
	virtual void setZFar(float z_far) = 0;

	virtual const float* getClearColor(void) const = 0;
	virtual void setClearColor(float r, float g, float b, float a) = 0;
	virtual void setClearColor(const float* color) = 0;

	virtual ClearMode getClearMode(void) const = 0;
	virtual void setClearMode(ClearMode cm) = 0;

	virtual ResourceWrapper<RenderTargetData>& getRenderTarget(void) = 0;
	virtual const Array<unsigned int>& getDevices(void) const = 0;
	virtual unsigned char getRenderOrder(void) const = 0;

	SHIB_INTERFACE_REFLECTION(ICameraComponent)
	SHIB_INTERFACE_NAME("Camera Component")
};

NS_END
