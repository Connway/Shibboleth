/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_RasterState_OpenGL.h"
#include "Gleam_RenderDevice_OpenGL.h"

NS_GLEAM

RasterStateGL::RasterStateGL(void)
{
}

RasterStateGL::~RasterStateGL(void)
{
}

bool RasterStateGL::init(IRenderDevice& rd, const RasterStateSettings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	_raster_settings = settings;
	return true;
}

void RasterStateGL::destroy(void)
{
}

void RasterStateGL::set(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setRasterState(this);
}

void RasterStateGL::unset(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setRasterState(nullptr);
}

RendererType RasterStateGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

const IRasterState::RasterStateSettings& RasterStateGL::getRasterSettings(void) const
{
	return _raster_settings;
}

NS_END
