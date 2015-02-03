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

#include "Shibboleth_TextureLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_RenderManager.h"
#include <Gleam_IRenderDevice.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_Image.h>

NS_SHIBBOLETH

TextureLoader::TextureLoader(RenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

TextureLoader::~TextureLoader(void)
{
}

Gaff::IVirtualDestructor* TextureLoader::load(const char* file_name, unsigned long long user_data)
{
	Gaff::Image image;

	if (!image.init()) {
		_render_mgr.printfLoadLog("ERROR - Failed to initialize image data structure when loading '%s.'\n", file_name);
		return nullptr;
	}

	if (!image.load(file_name)) {
		_render_mgr.printfLoadLog("ERROR - Failed to image '%s.'\n", file_name);
		return nullptr;
	}

	if (image.getType() == Gaff::Image::TYPE_DOUBLE) {
		_render_mgr.printfLoadLog("ERROR - Image of type double not supported. IMAGE: %s\n", file_name);
		return nullptr;
	}

	const TextureUserData* tud = (TextureUserData*)user_data;

	Gleam::ITexture::FORMAT texture_format = determineFormatAndType(image, !!(tud->gud.flags & TEX_LOADER_NORMALIZED), !!(tud->gud.flags & TEX_LOADER_SRGBA));

	TextureData* texture_data = GetAllocator()->template allocT<TextureData>();

	if (!texture_data) {
		_render_mgr.printfLoadLog("ERROR - Failed to allocate texture data structure.\n", file_name);
		return nullptr;
	}

	if (tud->gud.flags & TEX_LOADER_IMAGE_ONLY) {
		texture_data->image = Gaff::Move(image);
		texture_data->normalized = false;
		texture_data->cubemap = false;
		return texture_data;
	}

	texture_data->normalized = (tud->gud.flags & TEX_LOADER_NORMALIZED) != 0;
	texture_data->cubemap = (tud->gud.flags & TEX_LOADER_CUBEMAP) != 0;

	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr.getSpinLock());
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	Array<const RenderManager::WindowData*> windows = (tud->gud.flags & TEX_LOADER_TAGS_ANY) ?
		_render_mgr.getAllWindowsWithTagsAny(tud->gud.display_tags) :
		_render_mgr.getAllWindowsWithTags(tud->gud.display_tags);

	assert(!windows.empty());

	texture_data->textures.resize(rd.getNumDevices());

	if (!(tud->gud.flags & TEX_LOADER_NO_SRVS)) {
		texture_data->resource_views.resize(rd.getNumDevices());
	}

	if (!(tud->gud.flags & TEX_LOADER_NO_SAMPLERS)) {
		texture_data->samplers.resize(rd.getNumDevices());
	}

	// Load the texture for each device
	for (auto it = windows.begin(); it != windows.end(); ++it) {
		TexturePtr texture(_render_mgr.createTexture());
		rd.setCurrentDevice((*it)->device);

		if (!texture) {
			_render_mgr.printfLoadLog("ERROR - Failed to allocate texture for image %s.\n", file_name);
			GetAllocator()->freeT(texture_data);
			return nullptr;
		}

		if (texture_format == Gleam::ITexture::FORMAT_SIZE) {
			_render_mgr.printfLoadLog("ERROR - Could not determine the pixel format of image at %s.\n", file_name);
			GetAllocator()->freeT(texture_data);
			return nullptr;
		}

		unsigned int width = image.getWidth();
		unsigned int height = image.getHeight();
		unsigned int depth = image.getDepth();
		bool success = false;

		if (tud->gud.flags & TEX_LOADER_CUBEMAP) {
			if (width == 1 || height == 1 || depth != 1) {
				_render_mgr.printfLoadLog("ERROR - Image specified as cubemap, but is not a 2D image. IMAGE: %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return nullptr;
			}

			if (!texture->initCubemap(rd, width, height, texture_format, 1, image.getBuffer())) {
				_render_mgr.printfLoadLog("ERROR - Failed to initialize cubemap texture using image at %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return nullptr;
			}

		} else {
			// 3D
			if (width > 1 && height > 1 && depth > 1) {
				success = texture->init3D(rd, width, height, depth, texture_format, 1, image.getBuffer());

				// 2D
			}
			else if (width > 1 && height > 1) {
				success = texture->init2D(rd, width, height, texture_format, 1, image.getBuffer());

				// 1D
			}
			else if (image.getWidth() > 1) {
				success = texture->init1D(rd, width, texture_format, 1, image.getBuffer());
			}
		}

		if (!success) {
			_render_mgr.printfLoadLog("ERROR - Failed to initialize texture using image at %s.\n", file_name);
			GetAllocator()->freeT(texture_data);
			return nullptr;
		}

		texture_data->textures[(*it)->device] = texture;

		if (!(tud->gud.flags & TEX_LOADER_NO_SRVS)) {
			ShaderResourceViewPtr srv(_render_mgr.createShaderResourceView());

			if (!srv) {
				_render_mgr.printfLoadLog("ERROR - Failed to allocate shader resource view for texture %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return false;
			}

			if (!srv->init(rd, texture.get())) {
				_render_mgr.printfLoadLog("ERROR - Failed to initialize shader resource view for texture %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return false;
			}

			texture_data->resource_views[(*it)->device] = Gaff::Move(srv);
		}

		if (!(tud->gud.flags & TEX_LOADER_NO_SAMPLERS)) {
			SamplerStatePtr sampler(_render_mgr.createSamplerState());

			if (!sampler) {
				_render_mgr.printfLoadLog("ERROR - Failed to allocate sampler state for texture %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return false;
			}

			if (!sampler->init(
					rd, tud->filter, tud->wrap_u, tud->wrap_v, tud->wrap_w,
					tud->min_lod, tud->max_lod, tud->lod_bias, tud->max_anisotropy,
					tud->border_r, tud->border_g, tud->border_b, tud->border_a)) {

				_render_mgr.printfLoadLog("ERROR - Failed to initialize sampler state for texture %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return false;
			}

			texture_data->samplers[(*it)->device] = Gaff::Move(sampler);
		}
	}

	return texture_data;
}

Gleam::ITexture::FORMAT TextureLoader::determineFormatAndType(const Gaff::Image& image, bool normalized, bool srgba) const
{
	switch (image.getFormat()) {
		case Gaff::Image::FMT_RGBA:
			return determineRGBAType(image, normalized, srgba);
			break;

		case Gaff::Image::FMT_RGB:
			return determineRGBType(image, normalized);
			break;

		case Gaff::Image::FMT_LUMINANCE:
			return determineGreyscaleType(image, normalized);
			break;

		case Gaff::Image::FMT_BGR:
			return determineBGRType(image, normalized);
			break;

		case Gaff::Image::FMT_BGRA:
			return determineBGRAType(image, normalized);
			break;
	}

	return Gleam::ITexture::FORMAT_SIZE;
}

Gleam::ITexture::FORMAT TextureLoader::determineRGBAType(const Gaff::Image& image, bool normalized, bool srgba) const
{
	switch (image.getType()) {
		case Gaff::Image::TYPE_BYTE:
			return (normalized) ? Gleam::ITexture::RGBA_8_SNORM : Gleam::ITexture::RGBA_8_I;

		case Gaff::Image::TYPE_UNSIGNED_BYTE:
			return (srgba) ? Gleam::ITexture::SRGBA_8_UNORM : (normalized) ? Gleam::ITexture::RGBA_8_UNORM : Gleam::ITexture::RGBA_8_UI;

		case Gaff::Image::TYPE_SHORT:
			return Gleam::ITexture::RGBA_16_I;

		case Gaff::Image::TYPE_UNSIGNED_SHORT:
			return (normalized) ? Gleam::ITexture::RGBA_16_UNORM : Gleam::ITexture::RGBA_16_UI;

		case Gaff::Image::TYPE_INT:
			return Gleam::ITexture::RGBA_32_I;

		case Gaff::Image::TYPE_UNSIGNED_INT:
			return Gleam::ITexture::RGBA_32_UI;

		case Gaff::Image::TYPE_HALF:
			return Gleam::ITexture::RGBA_16_F;

		case Gaff::Image::TYPE_FLOAT:
			return Gleam::ITexture::RGBA_32_F;

		case Gaff::Image::TYPE_DOUBLE:
			// we do not support images of type double
			return Gleam::ITexture::FORMAT_SIZE;
	}

	return Gleam::ITexture::FORMAT_SIZE;
}

Gleam::ITexture::FORMAT TextureLoader::determineRGBType(const Gaff::Image& image, bool) const
{
	switch (image.getType()) {
		case Gaff::Image::TYPE_BYTE:
		case Gaff::Image::TYPE_UNSIGNED_BYTE:
			// we do not support 
			return Gleam::ITexture::FORMAT_SIZE;

		case Gaff::Image::TYPE_SHORT:
			return Gleam::ITexture::RGB_16_SNORM;

		case Gaff::Image::TYPE_UNSIGNED_SHORT:
			// we do not support 
			return Gleam::ITexture::FORMAT_SIZE;

		case Gaff::Image::TYPE_INT:
			return Gleam::ITexture::RGB_32_I;

		case Gaff::Image::TYPE_UNSIGNED_INT:
			return Gleam::ITexture::RGB_32_UI;

		case Gaff::Image::TYPE_HALF:
			return Gleam::ITexture::RGB_16_F;

		case Gaff::Image::TYPE_FLOAT:
			return Gleam::ITexture::RGB_32_F;

		case Gaff::Image::TYPE_DOUBLE:
			// we do not support images of type double
			return Gleam::ITexture::FORMAT_SIZE;
	}

	return Gleam::ITexture::FORMAT_SIZE;
}

Gleam::ITexture::FORMAT TextureLoader::determineBGRAType(const Gaff::Image&, bool) const
{
	return Gleam::ITexture::FORMAT_SIZE;
}

Gleam::ITexture::FORMAT TextureLoader::determineBGRType(const Gaff::Image&, bool) const
{
	return Gleam::ITexture::FORMAT_SIZE;
}

Gleam::ITexture::FORMAT TextureLoader::determineGreyscaleType(const Gaff::Image& image, bool normalized) const
{
	switch (image.getType()) {
		case Gaff::Image::TYPE_BYTE:
			return (normalized) ? Gleam::ITexture::R_8_SNORM : Gleam::ITexture::R_8_I;

		case Gaff::Image::TYPE_UNSIGNED_BYTE:
			return (normalized) ? Gleam::ITexture::R_8_SNORM : Gleam::ITexture::R_8_I;

		case Gaff::Image::TYPE_SHORT:
			return (normalized) ? Gleam::ITexture::R_16_SNORM : Gleam::ITexture::R_16_I;

		case Gaff::Image::TYPE_UNSIGNED_SHORT:
			return (normalized) ? Gleam::ITexture::R_16_UNORM : Gleam::ITexture::R_16_UI;

		case Gaff::Image::TYPE_INT:
			return Gleam::ITexture::R_32_I;

		case Gaff::Image::TYPE_UNSIGNED_INT:
			return Gleam::ITexture::R_32_UI;

		case Gaff::Image::TYPE_HALF:
			return Gleam::ITexture::R_16_F;

		case Gaff::Image::TYPE_FLOAT:
			return Gleam::ITexture::R_32_F;

		case Gaff::Image::TYPE_DOUBLE:
			// we do not support images of type double
			return Gleam::ITexture::FORMAT_SIZE;
	}

	return Gleam::ITexture::FORMAT_SIZE;
}

NS_END
