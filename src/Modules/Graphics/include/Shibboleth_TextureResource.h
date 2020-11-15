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

#pragma once

#include <Shibboleth_IResource.h>
#include <Shibboleth_SmartPtrs.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_ITexture.h>

NS_SHIBBOLETH

class Image;

class TextureResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Vector<Gleam::IRenderDevice*> getDevices(void) const;

	bool createTexture(const Vector<Gleam::IRenderDevice*>& devices, const Image& image, int32_t mip_levels = 1, bool make_linear = false);
	bool createTexture(Gleam::IRenderDevice& device, const Image& image, int32_t mip_levels = 1, bool make_linear = false);

	const Gleam::ITexture* getTexture(const Gleam::IRenderDevice& rd) const;
	Gleam::ITexture* getTexture(const Gleam::IRenderDevice& rd);

	const Gleam::IShaderResourceView* getShaderResourceView(const Gleam::IRenderDevice& rd) const;
	Gleam::IShaderResourceView* getShaderResourceView(const Gleam::IRenderDevice& rd);

private:
	using Data = eastl::pair< UniquePtr<Gleam::ITexture>, UniquePtr<Gleam::IShaderResourceView> >;
	VectorMap<const Gleam::IRenderDevice*, Data> _texture_data{ ProxyAllocator("Graphics") };

	void loadTexture(IFile* file, uintptr_t thread_id_int);
	void loadTextureJSON(const IFile* file, uintptr_t thread_id_int);
	void loadTextureImage(const IFile* file, const char* device_tag, const U8String& image_path, bool make_linear);

	SHIB_REFLECTION_CLASS_DECLARE(TextureResource);
};

using TextureResourcePtr = Gaff::RefPtr<TextureResource>;

NS_END

SHIB_REFLECTION_DECLARE(TextureResource)
