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

#include "Shibboleth_TextureLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_Image.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

class TextureLoaderFunctor : public ResourceLoaderFunctorBase
{
public:
	TextureLoaderFunctor(ResourceContainer* res_cont, TextureData* texture_data, TextureLoader* loader, const Gaff::JSON& json):
		_json(json), _texture_data(texture_data), _loader(loader), _res_cont(res_cont)
	{
	}

	~TextureLoaderFunctor(void) {}

	void operator()(ResourceContainer* res_cont)
	{
		if (res_cont->isLoaded()) {
			SingleDataWrapper<Gaff::Image>* image_data = res_cont->getResourcePtr< SingleDataWrapper<Gaff::Image> >();

			if (!_loader->finishLoadingResource(_res_cont->getResourceKey().getBuffer(), _texture_data, _json, image_data->data)) {
				SHIB_FREET(_texture_data, *GetAllocator());
				failedToLoadResource(_res_cont);
			}

			// Don't want to hold onto the CPU side of the image.
			clearSubResources(_res_cont);
			resourceLoadSuccessful(_res_cont);

		} else {
			failedToLoadResource(_res_cont);
		}
	}

private:
	Gaff::JSON _json;
	TextureData* _texture_data;
	TextureLoader* _loader;
	ResourceContainer* _res_cont;
};

TextureLoader::TextureLoader(IRenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

TextureLoader::~TextureLoader(void)
{
}

ResourceLoadData TextureLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	const char* file_name = res_cont->getResourceKey().getBuffer();
	LogManager& lm = GetApp().getLogManager();
	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to parse file '%s'.\n", file_name);
		return { nullptr };
	}

	if (!json.isObject()) {
		lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Texture file '%s' is malformed.\n", file_name);
		return { nullptr };
	}

	if (!json["image_file"].isString()) {
		lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Texture file '%s' is malformed. Value at 'image_file' is not a string.\n", file_name);
		return { nullptr };
	}

	TextureData* texture_data = SHIB_ALLOCT(TextureData, *GetAllocator());

	if (!texture_data) {
		lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate texture data structure.\n", file_name);
		return { nullptr };
	}

	ResourceLoadData res_load_data = { texture_data };
	res_load_data.sub_res_data.resize(1);
	res_load_data.sub_res_data[0] = {
		AString(json["image_file"].getString()),
		0,
		Gaff::Bind<TextureLoaderFunctor, void, ResourceContainer*>(TextureLoaderFunctor(res_cont, texture_data, this, json))
	};

	return res_load_data;
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

bool TextureLoader::finishLoadingResource(const char* file_name, TextureData* texture_data, const Gaff::JSON& json, const Gaff::Image& image)
{
	Gaff::JSON display_tags = json["display_tags"];
	LogManager& lm = GetApp().getLogManager();
	uint16_t disp_tags = 0;

	if (image.getType() == Gaff::Image::TYPE_DOUBLE) {
		lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Image of type double not supported. IMAGE: %s\n", json["image_file"].getString());
		return false;
	}

	if (!display_tags.isNull()) {
		if (!display_tags.isArray()) {
			lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Texture file '%s' is malformed. Value at 'display_tags' is not an array of strings.\n", file_name);
			return false;
		}

		disp_tags = _render_mgr.getDislayTags(display_tags);

		if (!disp_tags) {
			lm.logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"ERROR - Texture file '%s' is malformed (an element in 'display_tags' is not a string) or no tags found.\n",
				file_name
			);

			return false;
		}
	}

	Gleam::ITexture::FORMAT texture_format = determineFormatAndType(image, json["normalized"].isTrue(), json["SRGBA"].isTrue());

	texture_data->normalized = json["normalized"].isTrue();
	texture_data->cubemap = json["cubemap"].isTrue();

	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	Array<const IRenderManager::WindowData*> windows = (json["any_display_with_tags"].isTrue()) ?
		_render_mgr.getWindowsWithTagsAny(disp_tags) :
		_render_mgr.getWindowsWithTags(disp_tags);

	GAFF_ASSERT(!windows.empty());

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
			lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate texture for image %s.\n", file_name);
			SHIB_FREET(texture_data, *GetAllocator());
			return false;
		}

		if (texture_format == Gleam::ITexture::FORMAT_SIZE) {
			lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not determine the pixel format of image at %s.\n", file_name);
			SHIB_FREET(texture_data, *GetAllocator());
			return false;
		}

		unsigned int width = image.getWidth();
		unsigned int height = image.getHeight();
		unsigned int depth = image.getDepth();
		bool success = false;

		if (texture_data->cubemap) {
			if (width == 1 || height == 1 || depth != 1) {
				lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Image specified as cubemap, but is not a 2D image. IMAGE: %s.\n", file_name);
				return false;
			}

			if (!texture->initCubemap(rd, width, height, texture_format, 1, image.getBuffer())) {
				lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize cubemap texture using image at %s.\n", file_name);
				return false;
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
			lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize texture using image at %s.\n", file_name);
			return false;
		}

		texture_data->textures[(*it)->device] = texture;

		if (!json["no_shader_resource_views"].isTrue()) {
			ShaderResourceViewPtr srv(_render_mgr.createShaderResourceView());

			if (!srv) {
				lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate shader resource view for texture %s.\n", file_name);
				return false;
			}

			if (!srv->init(rd, texture.get())) {
				lm.logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize shader resource view for texture %s.\n", file_name);
				return false;
			}

			texture_data->resource_views[(*it)->device] = std::move(srv);
		}
	}

	return true;
}

NS_END
