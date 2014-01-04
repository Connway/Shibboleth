/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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
#include <Gaff_IncludeAssert.h>

NS_GLEAM

ShaderResourceViewGL::ShaderResourceViewGL(void):
	_resource(0), _target(0)
{
}

ShaderResourceViewGL::~ShaderResourceViewGL(void)
{
}

bool ShaderResourceViewGL::init(const IRenderDevice&, const ITexture* texture)
{
	assert(texture);
	const TextureGL* tex = (const TextureGL*)texture;
	_resource = tex->getTexture();
	_target = tex->getTexType();
	_view_type = VIEW_TEXTURE;
	return true;
}

void ShaderResourceViewGL::destroy(void)
{
	_resource = 0;
	_target = 0;
}

bool ShaderResourceViewGL::isD3D(void) const
{
	return false;
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
