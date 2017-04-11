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

#pragma once

#include "Gleam_Defines.h"
#include "Gaff_Defines.h"

NS_GLEAM

class DepthStencilStateGL;
class ProgramBuffersGL;
class RasterStateGL;
class BlendStateGL;
class ProgramGL;
class LayoutGL;
class MeshGL;
class IMesh;

class IRenderDeviceGL
{
public:
	IRenderDeviceGL(void) {}
	virtual ~IRenderDeviceGL(void) {}

	virtual void setDepthStencilState(const DepthStencilStateGL* ds_state) = 0;
	virtual void setRasterState(const RasterStateGL* raster_state) = 0;
	virtual void setBlendState(const BlendStateGL* blend_state) = 0;

	virtual void setLayout(LayoutGL* layout, const IMesh* mesh) = 0;
	virtual void unsetLayout(LayoutGL* layout) = 0;

	virtual void bindShader(ProgramGL* shader) = 0;
	virtual void unbindShader(void) = 0;

	virtual void bindProgramBuffers(ProgramBuffersGL* program_buffers) = 0;

	virtual void renderMeshNonIndexed(uint32_t topology, int32_t vert_count, int32_t start_location) = 0;
	virtual void renderMeshInstanced(MeshGL* mesh, int32_t count) = 0;
	virtual void renderMesh(MeshGL* mesh) = 0;
	virtual void renderNoVertexInput(int32_t vert_count) = 0;
};

NS_END
