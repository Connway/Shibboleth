/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Camera/Shibboleth_CameraPipelineData.h"
#include "Shibboleth_GraphicsLogging.h"
#include "Shibboleth_RenderManager.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Shibboleth_Math.h>
#include <Gleam_RenderOutput.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_Window.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::CameraPipelineData)
	.template ctor<>()

	.func("getView", static_cast<const Shibboleth::CameraView& (Shibboleth::CameraPipelineData::*)(int32_t) const>(&Shibboleth::CameraPipelineData::getView))
	.func("getView", static_cast<Shibboleth::CameraView& (Shibboleth::CameraPipelineData::*)(int32_t)>(&Shibboleth::CameraPipelineData::getView))

//	.func("getRenderData", static_cast<const Shibboleth::CameraRenderData& (Shibboleth::CameraPipelineData::*)(int32_t) const>(&Shibboleth::CameraPipelineData::getRenderData))
//	.func("getRenderData", static_cast<Shibboleth::CameraRenderData& (Shibboleth::CameraPipelineData::*)(int32_t)>(&Shibboleth::CameraPipelineData::getRenderData))

	//.func("createRenderData", &Shibboleth::CameraPipelineData::createRenderData)
SHIB_REFLECTION_DEFINE_END(Shibboleth::CameraPipelineData)

//SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::CameraRenderData)
//	.var("g_buffer", &Shibboleth::CameraRenderData::g_buffer)
//	.var("view", &Shibboleth::CameraRenderData::view)
//SHIB_REFLECTION_DEFINE_END(Shibboleth::CameraRenderData)

//SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::GBuffer)
//SHIB_REFLECTION_DEFINE_END(Shibboleth::GBuffer)


namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ GRAPHICS_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CameraPipelineData)

bool CameraPipelineData::init(RenderManager& render_mgr)
{
	// Create a g-buffer for each output window. 99.99% of the time, this will be a single window.
	for (int32_t i = 0; i < render_mgr.getNumWindows(); ++i) {
		Gleam::RenderOutput* const output = render_mgr.getOutput(i);
		Gleam::RenderDevice* const device = render_mgr.getDevice(*output);

		CameraRenderData render_data;
		GBuffer g_buffer;

		if (!createGBuffer(g_buffer, *device, output->getSize(), false)) {
			LogErrorGraphics("CameraPipelineData::createRenderData: Failed to create g-buffer for camera/window [%s].", render_mgr.getWindow(i)->getTitle());
			return false;
		}

		Gleam::IRenderTarget* const render_target = output->getRenderTarget();
		GAFF_ASSERT(render_target->getRendererType() == g_buffer.render_target->getRendererType());

		g_buffer.to_output_render_target = static_cast<Gleam::RenderTarget*>(render_target);

		render_data.g_buffers.emplace(device, std::move(g_buffer));
		_render_data.emplace(std::move(render_data));
	}

	return true;
}

const SparseStack<CameraRenderData>& CameraPipelineData::getRenderData(void) const
{
	return _render_data;
}

SparseStack<CameraRenderData>& CameraPipelineData::getRenderData(void)
{
	return _render_data;
}

const CameraRenderData& CameraPipelineData::getRenderData(int32_t id) const
{
	return const_cast<CameraPipelineData*>(this)->getRenderData(id);
}

CameraRenderData& CameraPipelineData::getRenderData(int32_t id)
{
	return _render_data[id];
}

const CameraView& CameraPipelineData::getView(int32_t id) const
{
	return const_cast<CameraPipelineData*>(this)->getView(id);
}

CameraView& CameraPipelineData::getView(int32_t id)
{
	return _render_data[id].view;
}

int32_t CameraPipelineData::createRenderData(
	Gaff::Hash32 device_tag,
	const Gleam::IVec2& size,
	bool create_render_texture)
{
	ManagerRef<RenderManager> render_mgr;
	const auto* const devices = render_mgr->getDevicesByTag(device_tag);

	if (!devices || devices->empty()) {
		LogErrorGraphics("CameraPipelineData::createRenderData: Failed to create diffuse texture for camera [%u].", device_tag);
		return -1;
	}

	CameraRenderData render_data;

	for (Gleam::RenderDevice* device : *devices) {
		GBuffer g_buffer;

		if (!createGBuffer(g_buffer, *device, size, create_render_texture)) {
			LogErrorGraphics("CameraPipelineData::createRenderData: Failed to create g-buffer for camera [%u].", device_tag);
			return -1;
		}

		render_data.g_buffers.emplace(device, std::move(g_buffer));
	}

	return _render_data.emplace(std::move(render_data));
}

void CameraPipelineData::removeRenderData(int32_t id)
{
	_render_data.remove(id);
}

bool CameraPipelineData::createGBuffer(GBuffer& g_buffer, Gleam::RenderDevice& rd, const Gleam::IVec2& size, bool create_render_texture)
{
	g_buffer.render_target.reset(SHIB_ALLOCT(Gleam::RenderTarget, s_allocator));

	g_buffer.diffuse.reset(SHIB_ALLOCT(Gleam::Texture, s_allocator));
	g_buffer.specular.reset(SHIB_ALLOCT(Gleam::Texture, s_allocator));
	g_buffer.normal.reset(SHIB_ALLOCT(Gleam::Texture, s_allocator));
	g_buffer.position.reset(SHIB_ALLOCT(Gleam::Texture, s_allocator));
	g_buffer.depth.reset(SHIB_ALLOCT(Gleam::Texture, s_allocator));

	g_buffer.diffuse_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator));
	g_buffer.specular_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator));
	g_buffer.normal_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator));
	g_buffer.position_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator));
	g_buffer.depth_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator));

	if (!g_buffer.diffuse->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create diffuse texture for camera.");
		return false;
	}

	if (!g_buffer.specular->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create specular texture for camera.");
		return false;
	}

	if (!g_buffer.normal->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create normal texture for camera.");
		return false;
	}

	if (!g_buffer.position->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create position texture for camera.");
		return false;
	}

	if (!g_buffer.depth->initDepthStencil(rd, size.x, size.y, Gleam::ITexture::Format::DEPTH_32_F)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create depth texture for camera.");
		return false;
	}

	if (!g_buffer.diffuse_srv->init(rd, *g_buffer.diffuse)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create diffuse srv for camera.");
		return false;
	}

	if (!g_buffer.specular_srv->init(rd, *g_buffer.specular)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create specular srv for camera.");
		return false;
	}

	if (!g_buffer.normal_srv->init(rd, *g_buffer.normal)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create normal srv for camera.");
		return false;
	}

	if (!g_buffer.position_srv->init(rd, *g_buffer.position)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create position srv for camera.");
		return false;
	}

	if (!g_buffer.depth_srv->init(rd, *g_buffer.depth)) {
		LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create depth srv for camera.");
		return false;
	}

	g_buffer.render_target->addTexture(rd, *g_buffer.specular);
	g_buffer.render_target->addTexture(rd, *g_buffer.diffuse);
	g_buffer.render_target->addTexture(rd, *g_buffer.normal);
	g_buffer.render_target->addTexture(rd, *g_buffer.position);
	g_buffer.render_target->addDepthStencilBuffer(rd, *g_buffer.depth);

	if (create_render_texture) {
		g_buffer.final_image.reset(SHIB_ALLOCT(Gleam::Texture, s_allocator));

		if (!g_buffer.final_image->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create final output texture for camera.");
			return false;
		}

		g_buffer.final_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator));

		if (!g_buffer.final_srv->init(rd, *g_buffer.final_image)) {
			LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create final output srv for camera.");
			return false;
		}

		g_buffer.final_render_target.reset(SHIB_ALLOCT(Gleam::RenderTarget, s_allocator));

		if (!g_buffer.final_render_target->addTexture(rd,* g_buffer.final_image)) {
			LogErrorGraphics("CameraPipelineData::createGBuffer: Failed to create final output render target for camera.");
			return false;
		}

		g_buffer.to_output_render_target = g_buffer.final_render_target.get();
	}

	return true;
}

NS_END
