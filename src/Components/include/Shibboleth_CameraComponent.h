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

#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceWrapper.h"
#include "Shibboleth_Component.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_Array.h>
#include <Gleam_Matrix4x4_CPU.h>
#include <Gleam_Frustum_CPU.h>

NS_SHIBBOLETH

class IApp;

class CameraComponent : public Component
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

	static const char* GetFriendlyName(void);

	CameraComponent(void);
	~CameraComponent(void);

	const Gaff::JSON& getSchema(void) const override;

	bool validate(const Gaff::JSON& json) override;
	bool load(const Gaff::JSON& json) override;
	bool save(Gaff::JSON& json) override;

	void updateFrustum(Object* object, uint64_t);

	INLINE const Gleam::Matrix4x4CPU& getProjectionMatrix(void) const;
	INLINE const Gleam::FrustumCPU& getFrustum(void) const;
	INLINE const float* getViewport(void) const;
	INLINE float getAspectRatio(void) const;
	INLINE float getFOV(void) const;
	INLINE void setFOV(float fov);
	INLINE float getZNear(void) const;
	INLINE void setZNear(float z_near);
	INLINE float getZFar(void) const;
	INLINE void setZFar(float z_far);

	INLINE const float* getClearColor(void) const;
	INLINE void setClearColor(float r, float g, float b, float a);
	INLINE void setClearColor(const float* color);

	INLINE ClearMode getClearMode(void) const;
	INLINE void setClearMode(ClearMode cm);

	INLINE ResourceWrapper<RenderTargetData>& getRenderTarget(void);
	INLINE const Array<unsigned int>& getDevices(void) const;
	INLINE unsigned char getRenderOrder(void) const;

	void setActive(bool active) override;

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

	SHIB_REF_DEF(CameraComponent);
	REF_DEF_REQ;
};

SHIB_ENUM_REF_DEF_EMBEDDED(ClearMode, CameraComponent::ClearMode);

NS_END
