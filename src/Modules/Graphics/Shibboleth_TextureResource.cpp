/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(TextureResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TextureResource)
	.classAttrs(
		CreatableAttribute(),
		ResExtAttribute(".bmp"),
		ResExtAttribute(".jpeg"),
		ResExtAttribute(".jpg"),
		ResExtAttribute(".png"),
		ResExtAttribute(".tga"),
		ResExtAttribute(".tiff"),
		ResExtAttribute(".tif"),
		MakeLoadFileCallbackAttribute(&TextureResource::loadTexture)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(TextureResource)

const Gleam::ITexture* TextureResource::getTexture(const Gleam::IRenderDevice& rd) const
{
	const auto it = _textures.find(&rd);
	return (it != _textures.end()) ? it->second.get() : nullptr;
}

Gleam::ITexture* TextureResource::getTexture(const Gleam::IRenderDevice& rd)
{
	const auto it = _textures.find(&rd);
	return (it != _textures.end()) ? it->second.get() : nullptr;
}

void TextureResource::loadTexture(IFile* file)
{
	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	GAFF_REF(file, render_mgr);
}

NS_END
