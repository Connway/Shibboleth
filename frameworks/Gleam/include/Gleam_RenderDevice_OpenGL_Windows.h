/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_IRenderDevice_OpenGL.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_BitVector.h"
#include "Gleam_VectorMap.h"
#include "Gleam_String.h"
#include <Gaff_IncludeWindows.h>
#include <mutex>

NS_GLEAM

class RenderDeviceGL : public IRenderDevice, public IRenderDeviceGL
{
public:
	static bool CheckRequiredExtensions(void);

	RenderDeviceGL(void);
	~RenderDeviceGL(void);

	bool init(int32_t adapter_id) override;

	void frameBegin(IRenderOutput& output) override;
	void frameEnd(IRenderOutput& output) override;

	bool isDeferred(void) const override;
	RendererType getRendererType(void) const override;

	IRenderDevice* createDeferredRenderDevice(void) override;
	void executeCommandList(ICommandList* command_list) override;
	bool finishCommandList(ICommandList* command_list) override;

	void resetRenderState(void) override;
	void renderNoVertexInput(int32_t vert_count) override;



	//// Common
	//void setClearColor(float r, float g, float b, float a) override;

	//void resetRenderState(void) override;

	//bool isDeferred(void) const override;
	//RendererType getRendererType(void) const override;

	//IRenderDevice* createDeferredRenderDevice(void) override;
	//void executeCommandList(ICommandList* command_list) override;
	//bool finishCommandList(ICommandList* command_list) override;

	//// For supporting deferred contexts
	//void setDepthStencilState(const DepthStencilStateGL* ds_state) override;
	//void setRasterState(const RasterStateGL* raster_state) override;
	//void setBlendState(const BlendStateGL* blend_state) override;

	//void setLayout(LayoutGL* layout, const IMesh* mesh) override;
	//void unsetLayout(LayoutGL* layout) override;

	//void bindShader(ProgramGL* shader) override;
	//void unbindShader(void) override;

	//void bindProgramBuffers(ProgramBuffersGL* program_buffers) override;

	//void renderMeshNonIndexed(uint32_t topology, int32_t vert_count, int32_t start_location) override;
	//void renderMeshInstanced(MeshGL* mesh, int32_t count) override;
	//void renderMesh(MeshGL* mesh) override;
	//void renderNoVertexInput(int32_t vert_count) override;

private:
	struct OutputInfo
	{
		Vector<DEVMODE> display_mode_list;
		U8String name;
	};

	struct AdapterInfo
	{
		Vector<OutputInfo> output_info;
		DISPLAY_DEVICE display_device;
		U8String name;
	};

	struct Viewport
	{
		int x;
		int y;
		int width;
		int height;
	};

	struct Device
	{
		Vector<HGLRC> contexts;
		Vector<HDC> outputs;
		Vector<Viewport> viewports;
		Vector<HWND> windows;
		Vector<IRenderTargetPtr> rts;
		BitVector vsync;

		int32_t adapter_id;
	};

	Vector<AdapterInfo> _display_info;
	Vector<Device> _devices;

	// Key is thread id, value is 2D array. First index is device, second is output.
	VectorMap< std::thread::id, Vector< Vector<HGLRC> > > _thread_contexts;

	// Fix this
	static THREAD_LOCAL const Viewport* _active_viewport;
	static THREAD_LOCAL HDC _active_output;

	static THREAD_LOCAL int32_t _curr_output;
	static THREAD_LOCAL int32_t _curr_device;

	bool _glew_already_initialized;
};

NS_END
