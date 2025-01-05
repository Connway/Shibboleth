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

#ifdef GLEAM_USE_VULKAN

#include "Vulkan/Gleam_Texture_Vulkan.h"

NS_GLEAM

Texture::~Texture(void)
{
}

void Texture::destroy(void)
{
}

bool Texture::init2DArray(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t num_elements, int32_t mip_levels, const void* buffer)
{
	GAFF_REF(rd, width, height, format, num_elements, mip_levels, buffer);
	return true;
}

bool Texture::init1DArray(IRenderDevice& rd, int32_t width, Format format, int32_t num_elements, int32_t mip_levels, const void* buffer)
{
	GAFF_REF(rd, width, format, num_elements, mip_levels, buffer);
	return true;
}

bool Texture::init3D(IRenderDevice& rd, int32_t width, int32_t height, int32_t depth, Format format, int32_t mip_levels, const void* buffer)
{
	GAFF_REF(rd, width, height, depth, format, mip_levels, buffer);
	return true;
}

bool Texture::init2D(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels, const void* buffer)
{
	GAFF_REF(rd, width, height, format, mip_levels, buffer);
	return true;
}

bool Texture::init1D(IRenderDevice& rd, int32_t width, Format format, int32_t mip_levels, const void* buffer)
{
	GAFF_REF(rd, width, format, mip_levels, buffer);
	return true;
}

bool Texture::initCubemap(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels, const void* buffer)
{
	GAFF_REF(rd, width, height, format, mip_levels, buffer);
	return true;
}

bool Texture::initDepthStencil(IRenderDevice& rd, int32_t width, int32_t height, Format format)
{
	GAFF_REF(rd, width, height, format);
	return true;
}

RendererType Texture::getRendererType(void) const
{
	return RendererType::Vulkan;
}

NS_END

#endif
