/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_ICameraComponent.h"
#include <Shibboleth_Component.h>
#include <Gleam_Matrix4x4_CPU.h>
#include <Gleam_Frustum_CPU.h>

NS_SHIBBOLETH

class CameraComponent : public Component, public ICameraComponent
{
public:
	CameraComponent(void);
	~CameraComponent(void);

	const Gaff::JSON& getSchema(void) const override;

	bool validate(const Gaff::JSON& json) override;
	bool load(const Gaff::JSON& json) override;
	bool save(Gaff::JSON& json) override;

	void setActive(bool active) override;

	const Gleam::Matrix4x4CPU& getProjectionMatrix(void) const override;
	const Gleam::FrustumCPU& getFrustum(void) const override;
	const float* getViewport(void) const override;
	float getAspectRatio(void) const override;
	float getFOV(void) const override;
	void setFOV(float fov) override;
	float getZNear(void) const override;
	void setZNear(float z_near) override;
	float getZFar(void) const override;
	void setZFar(float z_far) override;

	const float* getClearColor(void) const override;
	void setClearColor(float r, float g, float b, float a) override;
	void setClearColor(const float* color) override;

	ClearMode getClearMode(void) const override;
	void setClearMode(ClearMode cm) override;

	ResourceWrapper<RenderTargetData>& getRenderTarget(void) override;
	const Array<unsigned int>& getDevices(void) const override;
	unsigned char getRenderOrder(void) const override;

private:
	Gleam::Matrix4x4CPU _projection_matrix;
	Gleam::FrustumCPU _unstransformed_frustum;
	Gleam::FrustumCPU _frustum;

	mutable Array<unsigned int> _devices;

	ResourceWrapper<RenderTargetData> _render_target;

	ClearMode _clear_mode;
	float _clear_color[4];
	float _viewport[4];
	float _aspect_ratio;
	float _fov;
	float _z_near;
	float _z_far;
	unsigned char _render_order;

	void RenderTargetCallback(ResourceContainerBase*);
	void constructProjectionMatrixAndFrustum(void);
	void updateFrustum(Object* object, uint64_t);

	SHIB_REF_DEF(CameraComponent);
	REF_DEF_REQ;
};

SHIB_ENUM_REF_DEF_EMBEDDED(ClearMode, CameraComponent::ClearMode);

NS_END
