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

#include "Gleam_ShaderResourceView_OpenGL.h"
#include "Gleam_Texture_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include <Gaff_Assert.h>

NS_GLEAM

ShaderResourceViewGL::ShaderResourceViewGL(void):
	_resource(0), _target(0)
{
}

ShaderResourceViewGL::~ShaderResourceViewGL(void)
{
}

bool ShaderResourceViewGL::init(IRenderDevice&, const ITexture* texture)
{
	GAFF_ASSERT(texture);
	const TextureGL* tex = reinterpret_cast<const TextureGL*>(texture);
	_resource = tex->getTexture();
	_target = tex->getTexType();
	_view_type = VIEW_TEXTURE;
	return true;
}

bool ShaderResourceViewGL::init(IRenderDevice&, const IBuffer* buffer)
{
	GAFF_ASSERT(buffer);
	const BufferGL* buf = reinterpret_cast<const BufferGL*>(buffer);
	_resource = buf->getBuffer();
	_target = static_cast<unsigned int>(-1); // TODO: Fix this.
	_view_type = VIEW_BUFFER;
	return true;
}

void ShaderResourceViewGL::destroy(void)
{
	_resource = 0;
	_target = 0;
}

RendererType ShaderResourceViewGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

unsigned int ShaderResourceViewGL::getResourceView(void) const
{
	return _resource;
}

unsigned int ShaderResourceViewGL::getTarget(void) const
{
	return _target;
}

NS_END