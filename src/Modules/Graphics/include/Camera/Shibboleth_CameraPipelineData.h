/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Pipelines/Shibboleth_IRenderPipelineData.h"
#include "Camera/Shibboleth_CameraView.h"
#include "Shibboleth_GraphicsDefines.h"
#include <Containers/Shibboleth_SparseStack.h>
#include <Containers/Shibboleth_VectorMap.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_RenderTarget.h>
#include <Gleam_Texture.h>

NS_GLEAM
	class RenderDevice;
NS_END

NS_SHIBBOLETH

struct GBuffer final
{
	UniquePtr<Gleam::RenderTarget> render_target;

	UniquePtr<Gleam::Texture> diffuse;
	UniquePtr<Gleam::Texture> specular;
	UniquePtr<Gleam::Texture> normal;
	UniquePtr<Gleam::Texture> position;
	UniquePtr<Gleam::Texture> depth;

	UniquePtr<Gleam::ShaderResourceView> diffuse_srv;
	UniquePtr<Gleam::ShaderResourceView> specular_srv;
	UniquePtr<Gleam::ShaderResourceView> normal_srv;
	UniquePtr<Gleam::ShaderResourceView> position_srv;
	UniquePtr<Gleam::ShaderResourceView> depth_srv;

	// These fields are only filled out if doing off-screen rendering.
	UniquePtr<Gleam::RenderTarget> final_render_target;
	UniquePtr<Gleam::ShaderResourceView> final_srv;
	UniquePtr<Gleam::Texture> final_image;

	Gleam::RenderTarget* to_output_render_target = nullptr;
};

struct CameraRenderData final
{
	VectorMap<Gleam::RenderDevice*, GBuffer> g_buffers{ GRAPHICS_ALLOCATOR };
	CameraView view;
};

// $TODO: Camera's can be set to render specific scenes.
class CameraPipelineData final : public IRenderPipelineData
{
public:
	bool init(RenderManager& render_mgr) override;

	const SparseStack<CameraRenderData>& getRenderData(void) const;
	SparseStack<CameraRenderData>& getRenderData(void);

	const CameraRenderData& getRenderData(int32_t id) const;
	CameraRenderData& getRenderData(int32_t id);

	const CameraView& getView(int32_t id) const;
	CameraView& getView(int32_t id);

	int32_t createRenderData(
		Gaff::Hash32 device_tag,
		const Gleam::IVec2& size,
		// $TODO: Scenes that render to this g-buffer.
		bool create_render_texture = true
	);

	void removeRenderData(int32_t id);

private:
	SparseStack<CameraRenderData> _render_data{ GRAPHICS_ALLOCATOR };

	static bool createGBuffer(
		GBuffer& g_buffer,
		Gleam::RenderDevice& rd,
		const Gleam::IVec2& size,
		// $TODO: Scenes that render to this g-buffer.
		bool create_render_texture
	);

	SHIB_REFLECTION_CLASS_DECLARE(CameraPipelineData);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::CameraPipelineData)
//SHIB_REFLECTION_DECLARE(Shibboleth::CameraRenderData)
//SHIB_REFLECTION_DECLARE(Shibboleth::GBuffer)
