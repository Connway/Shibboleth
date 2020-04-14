/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_TextureResource.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Image.h>

SHIB_REFLECTION_DEFINE_BEGIN(TextureResource)
	.classAttrs(
		CreatableAttribute(),
		ResExtAttribute(".texture.bin"),
		ResExtAttribute(".texture"),
		ResExtAttribute(".png"),
		ResExtAttribute(".tiff"),
		ResExtAttribute(".tif"),
		MakeLoadFileCallbackAttribute(&TextureResource::loadTexture)
	)

	.base<IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(TextureResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(TextureResource)

static Gleam::ITexture::Format GetTextureFormat(const Image& image)
{
	switch (image.getBitDepth()) {
		case 8:
			switch (image.getNumChannels()) {
				case 1: return Gleam::ITexture::Format::R_8_UNORM;
				case 2: return Gleam::ITexture::Format::RG_8_UNORM;
				//case 3: return Gleam::ITexture::Format::RGB_8_UNORM;
				case 4: return Gleam::ITexture::Format::RGBA_8_UNORM;
				default:
					break;
			}
			break;

		case 16:
			switch (image.getNumChannels()) {
				case 1: return Gleam::ITexture::Format::R_16_UNORM;
				case 2: return Gleam::ITexture::Format::RG_16_UNORM;
				//case 3: return Gleam::ITexture::Format::RGB_16_UNORM;
				case 4: return Gleam::ITexture::Format::RGBA_16_UNORM;
				default:
					break;
			}
			break;

		//case 32:
		//	break;

		default:
			break;
	}

	return Gleam::ITexture::Format::SIZE;
}

Vector<Gleam::IRenderDevice*> TextureResource::getDevices(void) const
{
	Vector<Gleam::IRenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _texture_data) {
		out.emplace_back(const_cast<Gleam::IRenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool TextureResource::createTexture(const Vector<Gleam::IRenderDevice*>& devices, const Image& image, int32_t mip_levels, bool make_linear)
{
	bool success = true;

	for (Gleam::IRenderDevice* device : devices) {
		success = success && createTexture(*device, image, mip_levels, make_linear);
	}

	return success;
}

bool TextureResource::createTexture(Gleam::IRenderDevice& device, const Image& image, int32_t mip_levels, bool make_linear)
{
	const IRenderManager& render_mgr = GetApp().GETMANAGERT(IRenderManager, RenderManager);
	Gleam::ITexture* const texture = render_mgr.createTexture();
	const Gleam::ITexture::Format format = GetTextureFormat(image);

	bool success = texture->init2D(
		device,
		image.getWidth(),
		image.getHeight(),
		(make_linear) ? Gleam::ITexture::GetSRGBFormat(format) : format,
		mip_levels,
		image.getBuffer()
	);

	if (!success) {
		LogErrorResource("Failed to create texture '%s'.", getFilePath().getBuffer());
		SHIB_FREET(texture, GetAllocator());
		return false;
	}

	Gleam::IShaderResourceView* const srv = render_mgr.createShaderResourceView();
	success = srv->init(device, texture);

	if (!success) {
		LogErrorResource("Failed to create texture shader resource view '%s'.", getFilePath().getBuffer());
		SHIB_FREET(texture, GetAllocator());
		SHIB_FREET(srv, GetAllocator());
		return false;
	}

	auto& data = _texture_data[&device];
	data.first.reset(texture);
	data.second.reset(srv);

	return true;
}

const Gleam::ITexture* TextureResource::getTexture(const Gleam::IRenderDevice& rd) const
{
	const auto it = _texture_data.find(&rd);
	return (it != _texture_data.end()) ? it->second.first.get() : nullptr;
}

Gleam::ITexture* TextureResource::getTexture(const Gleam::IRenderDevice& rd)
{
	const auto it = _texture_data.find(&rd);
	return (it != _texture_data.end()) ? it->second.first.get() : nullptr;
}

const Gleam::IShaderResourceView* TextureResource::getShaderResourceView(const Gleam::IRenderDevice& rd) const
{
	const auto it = _texture_data.find(&rd);
	return (it != _texture_data.end()) ? it->second.second.get() : nullptr;
}

Gleam::IShaderResourceView* TextureResource::getShaderResourceView(const Gleam::IRenderDevice& rd)
{
	const auto it = _texture_data.find(&rd);
	return (it != _texture_data.end()) ? it->second.second.get() : nullptr;
}

void TextureResource::loadTexture(IFile* file)
{
	if (Gaff::EndsWith(getFilePath().getBuffer(), ".texture") || Gaff::EndsWith(getFilePath().getBuffer(), ".texture.bin")) {
		loadTextureJSON(file);
		GetApp().getFileSystem().closeFile(file);
	} else {
		// Create SRV as is. No SRGB conversion.
		loadTextureImage(file, "main", getFilePath().getString(), false);
	}
}

void TextureResource::loadTextureJSON(const IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load material '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();
	const bool make_linear = reader.readBool("make_linear", false);
	U8String device_tag;

	{
		const auto guard = reader.enterElementGuard("devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed texture '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char* const tag = reader.readString("main");
		device_tag = tag;
		reader.freeString(tag);
	}

	const IFile* image_file = nullptr;
	U8String image_path;

	{
		const auto guard = reader.enterElementGuard("image");

		if (reader.isNull() || !reader.isString()) {
			LogErrorResource("Malformed texture '%s'. 'image' is not string.", getFilePath().getBuffer());
			failed();
			return;
 		}

		const char* const path = reader.readString();
		image_file = res_mgr.loadFileAndWait(path);
		image_path = path;
		reader.freeString(path);
	}

	if (!image_file) {
		LogErrorResource("Failed to load texture '%s'. 'image' does not point to a valid image file.", getFilePath().getBuffer());
		failed();
		return;
	}

	loadTextureImage(image_file, device_tag.data(), image_path, make_linear);
}

void TextureResource::loadTextureImage(const IFile* file, const char* device_tag, const U8String& image_path, bool make_linear)
{
	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	const Vector<Gleam::IRenderDevice*>* const devices = render_mgr.getDevicesByTag(device_tag);

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load texture '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer(), device_tag);
		failed();
		return;
	}

	const size_t index = image_path.find_last_of('.');
	Image image;

	if (!image.load(file->getBuffer(), file->size(), image_path.data() + index)) {
		LogErrorResource("Failed to load texture '%s'. Could not read or parse image file '%s'.", getFilePath().getBuffer(), image_path.data());
		failed();
		return;
	}

	if (createTexture(*devices, image, 1, make_linear)) {
		succeeded();
	} else {
		failed();
	}
}

NS_END
