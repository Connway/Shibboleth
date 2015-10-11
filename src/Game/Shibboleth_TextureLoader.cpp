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
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_TaskPoolTags.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_ScopedExit.h>
#include <Gaff_Image.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

TextureLoader::TextureLoader(RenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

TextureLoader::~TextureLoader(void)
{
}

Gaff::IVirtualDestructor* TextureLoader::load(const char* file_name, unsigned long long, HashMap<AString, IFile*>& file_map)
{
	assert(file_map.hasElementWithKey(AString(file_name)));

	GAFF_SCOPE_EXIT([&]()
	{
		for (auto it = file_map.begin(); it != file_map.end(); ++it) {
			if (*it) {
				GetApp().getFileSystem()->closeFile(*it);
				*it = nullptr;
			}
		}
	});

	IFile* file = file_map[AString(file_name)];

	Gaff::JSON json;
	
	if (!json.parse(file->getBuffer())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to parse file '%s'.\n", file_name);
		return nullptr;
	}

	if (!json.isObject()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Texture file '%s' is malformed.\n", file_name);
		return nullptr;
	}

	if (!json["image_file"].isString()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Texture file '%s' is malformed. Value at 'image_file' is not a string.\n", file_name);
		return nullptr;
	}

	Gaff::JSON display_tags = json["display_tags"];
	unsigned short disp_tags = 0;

	if (!display_tags.isNull()) {
		if (!display_tags.isArray()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Texture file '%s' is malformed. Value at 'display_tags' is not an array of strings.\n", file_name);
			return nullptr;
		}

		if (EXTRACT_DISPLAY_TAGS(display_tags, disp_tags)) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Texture file '%s' is malformed. An element in 'display_tags' is not a string.\n", file_name);
			return nullptr;
		}
	}

	assert(file_map.hasElementWithKey(AString(json["image_file"].getString())));
	file = file_map[AString(json["image_file"].getString())];

	Gaff::Image image;

	if (!image.init()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize image data structure when loading '%s'.\n", file_name);
		return nullptr;
	}

	if (!image.load(file->getBuffer(), static_cast<unsigned int>(file->size()))) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to load image '%s' while processing '%s'.\n", json["image_file"].getString(), file_name);
		return nullptr;
	}

	if (image.getType() == Gaff::Image::TYPE_DOUBLE) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Image of type double not supported. IMAGE: %s\n", json["image_file"].getString());
		return nullptr;
	}

	Gleam::ITexture::FORMAT texture_format = determineFormatAndType(image, json["normalized"].isTrue(), json["SRGBA"].isTrue());

	TextureData* texture_data = GetAllocator()->template allocT<TextureData>();

	if (!texture_data) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate texture data structure.\n", file_name);
		return nullptr;
	}

	if (json["image_only"].isTrue()) {
		texture_data->image = std::move(image);
		texture_data->normalized = false;
		texture_data->cubemap = false;
		return texture_data;
	}

	texture_data->normalized = json["normalized"].isTrue();
	texture_data->cubemap = json["cubemap"].isTrue();

	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	Array<const RenderManager::WindowData*> windows = (json["any_display_with_tags"].isTrue()) ?
		_render_mgr.getWindowsWithTagsAny(disp_tags) :
		_render_mgr.getWindowsWithTags(disp_tags);

	assert(!windows.empty());

	texture_data->textures.resize(rd.getNumDevices());

	if (!json["no_shader_resource_views"].isTrue()) {
		texture_data->resource_views.resize(rd.getNumDevices());
	}

	// Load the texture for each device
	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (texture_data->textures[(*it)->device]) {
			continue;
		}

		TexturePtr texture(_render_mgr.createTexture());
		rd.setCurrentDevice((*it)->device);

		if (!texture) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate texture for image %s.\n", file_name);
			GetAllocator()->freeT(texture_data);
			return nullptr;
		}

		if (texture_format == Gleam::ITexture::FORMAT_SIZE) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Could not determine the pixel format of image at %s.\n", file_name);
			GetAllocator()->freeT(texture_data);
			return nullptr;
		}

		unsigned int width = image.getWidth();
		unsigned int height = image.getHeight();
		unsigned int depth = image.getDepth();
		bool success = false;

		if (texture_data->cubemap) {
			if (width == 1 || height == 1 || depth != 1) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Image specified as cubemap, but is not a 2D image. IMAGE: %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return nullptr;
			}

			if (!texture->initCubemap(rd, width, height, texture_format, 1, image.getBuffer())) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize cubemap texture using image at %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return nullptr;
			}

		} else {
			// 3D
			if (width > 1 && height > 1 && depth > 1) {
				success = texture->init3D(rd, width, height, depth, texture_format, 1, image.getBuffer());

			// 2D
			} else if (width > 1 && height > 1) {
				success = texture->init2D(rd, width, height, texture_format, 1, image.getBuffer());

			// 1D
			} else if (image.getWidth() > 1) {
				success = texture->init1D(rd, width, texture_format, 1, image.getBuffer());
			}
		}

		if (!success) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize texture using image at %s.\n", file_name);
			GetAllocator()->freeT(texture_data);
			return nullptr;
		}

		texture_data->textures[(*it)->device] = texture;

		if (!json["no_shader_resource_views"].isTrue()) {
			ShaderResourceViewPtr srv(_render_mgr.createShaderResourceView());

			if (!srv) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate shader resource view for texture %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return false;
			}

			if (!srv->init(rd, texture.get())) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize shader resource view for texture %s.\n", file_name);
				GetAllocator()->freeT(texture_data);
				return false;
			}

			texture_data->resource_views[(*it)->device] = std::move(srv);
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
