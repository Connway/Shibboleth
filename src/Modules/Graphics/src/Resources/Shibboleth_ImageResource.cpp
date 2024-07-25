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

#include "Resources/Shibboleth_ImageResource.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_IFileSystem.h>
#include <GLFW/glfw3.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ImageResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResourceExtensionAttribute(u8".png"),
		Shibboleth::ResourceExtensionAttribute(u8".tiff"),
		Shibboleth::ResourceExtensionAttribute(u8".tif")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ImageResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ImageResource)

void ImageResource::load(const IFile& file, uintptr_t /*thread_id_int*/)
{
	const size_t index = getFilePath().getString().rfind('.');

	if (!_image.load(file.getBuffer(), file.size(), getFilePath().getString().data() + index)) {
		LogErrorResource("ImageResource::load: Could not read or parse image file '%s'.", getFilePath().getBuffer());

		failed();
		return;
	}

	succeeded();
}

void ImageResource::fillGLFWImage(GLFWimage& image) const
{
	image.width = _image.getWidth();
	image.height = _image.getHeight();
	image.pixels = const_cast<uint8_t*>(_image.getBuffer());
}

const Image& ImageResource::getImage(void) const
{
	return _image;
}

NS_END
