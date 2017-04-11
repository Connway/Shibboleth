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

#include "Gleam_IRenderDevice_OpenGL.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_BitVector.h"
#include "Gleam_String.h"
#include <GL/glxew.h>

NS_GLEAM

class RenderDeviceGL : public IRenderDevice, public IRenderDeviceGL
{
public:
	static bool CheckRequiredExtensions(void);

	RenderDeviceGL(void);
	~RenderDeviceGL(void);

	AdapterList getDisplayModes(int compat = 28) override;

	bool init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync = false) override;
	void destroy(void) override;

	bool isVsync(unsigned int device, unsigned int output) const override;
	void setVsync(bool vsync, unsigned int device, unsigned int output) override;

	void beginFrame(void) override;
	void endFrame(void) override;

	bool resize(const IWindow& window) override;
	bool handleFocusGained(const IWindow&) override;

	unsigned int getViewportWidth(unsigned int device, unsigned int output) const override;
	unsigned int getViewportHeight(unsigned int device, unsigned int output) const override;

	unsigned int getActiveViewportWidth(void) override;
	unsigned int getActiveViewportHeight(void) override;

	unsigned int getNumOutputs(unsigned int device) const override;
	unsigned int getNumDevices(void) const override;

	IRenderTargetPtr getOutputRenderTarget(unsigned int device, unsigned int output) override;
	IRenderTargetPtr getActiveOutputRenderTarget(void) override;

	bool setCurrentOutput(unsigned int output) override;
	unsigned int getCurrentOutput(void) const override;

	bool setCurrentDevice(unsigned int device) override;
	unsigned int getCurrentDevice(void) const override;

	unsigned int getDeviceForAdapter(unsigned int adapter_id) const override;
	unsigned int getDeviceForMonitor(unsigned int monitor) const override;

	// Common
	void setClearColor(float r, float g, float b, float a) override;

	void resetRenderState(void) override;

	bool isDeferred(void) const override;
	RendererType getRendererType(void) const override;

	IRenderDevice* createDeferredRenderDevice(void) override;
	void executeCommandList(ICommandList* command_list) override;
	bool finishCommandList(ICommandList* command_list) override;

	void renderNoVertexInput(unsigned int vert_count) override;

	// For supporting deferred contexts
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
	struct ScreenMode
	{
		int width;
		int height;
		short refresh_rate;
	};

	struct OutputInfo
	{
		Vector<ScreenMode> display_mode_list; // figure out xrandr data structure
		U8String name;
	};

	struct AdapterInfo
	{
		Vector<OutputInfo> output_info;
		//DISPLAY_DEVICE display_device; // figure out xrandr data structure
		//GleamGString name;
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
		Vector<GLXContext> contexts;
		Vector<Viewport> viewports;
		Vector<Window*> windows;
		Vector<IRenderTargetPtr> rts;
		BitVector vsync;

		unsigned int adapter_id;
	};

	Vector<AdapterInfo> _display_info;
	Vector<Device> _devices;

	const Viewport* _active_viewport;
	Window* _active_window;

	unsigned int _curr_output;
	unsigned int _curr_device;

	bool _glew_already_initialized;
};

NS_END
