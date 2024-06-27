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
#include <Gleam_RenderDevice.h>

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



NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CameraPipelineData)


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
		LogErrorGraphics("Failed to create diffuse texture for camera [%u].", device_tag);
		return -1;
	}

	ProxyAllocator allocator = GRAPHICS_ALLOCATOR;
	CameraRenderData render_data;

	for (Gleam::RenderDevice* device : *devices) {
		GBuffer g_buffer;

		g_buffer.render_target.reset(SHIB_ALLOCT(Gleam::RenderTarget, allocator));

		g_buffer.diffuse.reset(SHIB_ALLOCT(Gleam::Texture, allocator));
		g_buffer.specular.reset(SHIB_ALLOCT(Gleam::Texture, allocator));
		g_buffer.normal.reset(SHIB_ALLOCT(Gleam::Texture, allocator));
		g_buffer.position.reset(SHIB_ALLOCT(Gleam::Texture, allocator));
		g_buffer.depth.reset(SHIB_ALLOCT(Gleam::Texture, allocator));

		g_buffer.diffuse_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, allocator));
		g_buffer.specular_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, allocator));
		g_buffer.normal_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, allocator));
		g_buffer.position_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, allocator));
		g_buffer.depth_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, allocator));

		if (!g_buffer.diffuse->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create diffuse texture for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.specular->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create specular texture for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.normal->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create normal texture for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.position->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create position texture for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.depth->initDepthStencil(*device, size.x, size.y, Gleam::ITexture::Format::DEPTH_32_F)) {
			LogErrorGraphics("Failed to create depth texture for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.diffuse_srv->init(*device, *g_buffer.diffuse)) {
			LogErrorGraphics("Failed to create diffuse srv for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.specular_srv->init(*device, *g_buffer.specular)) {
			LogErrorGraphics("Failed to create specular srv for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.normal_srv->init(*device, *g_buffer.normal)) {
			LogErrorGraphics("Failed to create normal srv for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.position_srv->init(*device, *g_buffer.position)) {
			LogErrorGraphics("Failed to create position srv for camera [%u].", device_tag);
			return -1;
		}

		if (!g_buffer.depth_srv->init(*device, *g_buffer.depth)) {
			LogErrorGraphics("Failed to create depth srv for camera [%u].", device_tag);
			return -1;
		}

		g_buffer.render_target->addTexture(*device, *g_buffer.specular);
		g_buffer.render_target->addTexture(*device, *g_buffer.diffuse);
		g_buffer.render_target->addTexture(*device, *g_buffer.normal);
		g_buffer.render_target->addTexture(*device, *g_buffer.position);
		g_buffer.render_target->addDepthStencilBuffer(*device, *g_buffer.depth);

		if (create_render_texture) {
			g_buffer.final_image.reset(SHIB_ALLOCT(Gleam::Texture, allocator));

			if (!g_buffer.final_image->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
				LogErrorGraphics("Failed to create final output texture for camera [%u].", device_tag);
				return -1;
			}

			g_buffer.final_srv.reset(SHIB_ALLOCT(Gleam::ShaderResourceView, allocator));

			if (!g_buffer.final_srv->init(*device, *g_buffer.final_image)) {
				LogErrorGraphics("Failed to create final output srv for camera [%u].", device_tag);
				return -1;
			}

			g_buffer.final_render_target.reset(SHIB_ALLOCT(Gleam::RenderTarget, allocator));

			if (!g_buffer.final_render_target->addTexture(*device,* g_buffer.final_image)) {
				LogErrorGraphics("Failed to create final output render target for camera [%u].", device_tag);
				return -1;
			}
		}

		render_data.g_buffers.emplace(device, std::move(g_buffer));
	}

	return _render_data.emplace(std::move(render_data));
}

void CameraPipelineData::removeRenderData(int32_t id)
{
	_render_data.remove(id);
}

NS_END
