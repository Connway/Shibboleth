/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

namespace Gaff
{
	class Image;
}

NS_SHIBBOLETH

class RenderManager;

class TextureLoader : public IResourceLoader
{
public:
	TextureLoader(RenderManager& render_mgr);
	~TextureLoader(void);

	Gaff::IVirtualDestructor* load(const char* file_name, unsigned long long user_data);

private:
	RenderManager& _render_mgr;

	Gleam::ITexture::FORMAT determineFormatAndType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineRGBAType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineRGBType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineBGRAType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineBGRType(const Gaff::Image& image, bool normalized) const;
	Gleam::ITexture::FORMAT determineGreyscaleType(const Gaff::Image& image, bool normalized) const;

	GAFF_NO_COPY(TextureLoader);
	GAFF_NO_MOVE(TextureLoader);
};

NS_END