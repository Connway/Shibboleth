/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

	void destroy(void);
	bool isDeferred(void) const;
	bool isD3D(void) const;

	void executeCommandList(ICommandList* command_list);
	bool finishCommandList(ICommandList* command_list);

	void setRenderState(const RenderStateGL* render_state);

	void setLayout(LayoutGL* layout, const IMesh* mesh);
	void unsetLayout(LayoutGL* layout);

	void bindShader(ProgramGL* shader, ProgramBuffersGL* program_buffers);
	void unbindShader(void);

	void renderMeshNonIndexed(unsigned int topology, unsigned int vert_count, unsigned int start_location);
	void renderMeshInstanced(MeshGL* mesh, unsigned int count);
	void renderMesh(MeshGL* mesh);

private:
	CommandListGL _command_list;

	DeferredRenderDeviceGL(void);
	friend class RenderDeviceGL;
};

NS_END
