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

#include "Gleam_DeferredRenderDeviceBase.h"
#include "Gleam_IRenderDevice_OpenGL.h"
#include "Gleam_CommandList_OpenGL.h"

NS_GLEAM

class DeferredRenderDeviceGL: public DeferredRenderDeviceBase, public IRenderDeviceGL
{
public:
	~DeferredRenderDeviceGL(void);

	void destroy(void) override;
	bool isDeferred(void) const override;
	RendererType getRendererType(void) const override;

	void executeCommandList(ICommandList* command_list) override;
	bool finishCommandList(ICommandList* command_list) override;

	void renderNoVertexInput(unsigned int vert_count) override;

	void setDepthStencilState(const DepthStencilStateGL* ds_state) override;
	void setRasterState(const RasterStateGL* raster_state) override;
	void setBlendState(const BlendStateGL* blend_state) override;

	void setLayout(LayoutGL* layout, const IMesh* mesh) override;
	void unsetLayout(LayoutGL* layout) override;

	void bindShader(ProgramGL* shader) override;
	void unbindShader(void) override;

	void bindProgramBuffers(ProgramBuffersGL* program_buffers) override;

	void renderMeshNonIndexed(unsigned int topology, unsigned int vert_count, unsigned int start_location) override;
	void renderMeshInstanced(MeshGL* mesh, unsigned int count) override;
	void renderMesh(MeshGL* mesh) override;

private:
	CommandListGL _command_list;

	DeferredRenderDeviceGL(void);
	friend class RenderDeviceGL;
};

NS_END
