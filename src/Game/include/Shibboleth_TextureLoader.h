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

#include "Shibboleth_IResourceLoader.h"
#include <Gleam_ITexture.h>

NS_GAFF
	class Image;
	class JSON;
NS_END

NS_SHIBBOLETH

class IRenderManager;
class IFileSystem;
struct TextureData;

class TextureLoader : public IResourceLoader
{
public:
	TextureLoader(IRenderManager& render_mgr);
	~TextureLoader(void);

	ResourceLoadData load(const IFile* file, ResourceContainer* res_cont) override;

private:
	IRenderManager& _render_mgr;

	Gleam::ITexture::FORMAT determineFormatAndType(const Gaff::Image& image, bool normalized, bool srgba) const;
	Gleam::ITexture::FORMAT determineRGBAType(const Gaff::Image& image, bool normalized, bool srgba) const;
	Gleam::ITexture::FORMAT determineRGBType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineBGRAType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineBGRType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineGreyscaleType(const Gaff::Image& image, bool normalized) const;

	bool finishLoadingResource(const char* file_name, TextureData* texture_data, const Gaff::JSON& json, const Gaff::Image& image);

	GAFF_NO_COPY(TextureLoader);
	GAFF_NO_MOVE(TextureLoader);

	friend class TextureLoaderFunctor;
};

NS_END
