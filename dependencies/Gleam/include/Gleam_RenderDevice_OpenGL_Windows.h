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

#include "Gleam_IRenderDevice_OpenGL.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_BitArray.h"
#include "Gleam_String.h"
#include "Gleam_Map.h"
#include <Gaff_IncludeWindows.h>
#include <Gaff_SpinLock.h>

NS_GLEAM

class RenderDeviceGL : public IRenderDevice, public IRenderDeviceGL
{
public:
	RenderDeviceGL(void);
	~RenderDeviceGL(void);

	static bool CheckRequiredExtensions(void);

	AdapterList getDisplayModes(int compat = 28/*29*/);

	bool initThreadData(unsigned int* thread_ids, size_t num_ids);
	bool init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync = false);
	void destroy(void);

	bool isVsync(unsigned int device, unsigned int output) const;
	void setVsync(bool vsync, unsigned int device, unsigned int output);

	void setClearColor(float r, float g, float b, float a);

	void beginFrame(void);
	void endFrame(void);

	bool resize(const IWindow& window);
	bool handleFocusGained(const IWindow&);

	void resetRenderState(void);

	bool isDeferred(void) const;
	bool isD3D(void) const;

	unsigned int getViewportWidth(unsigned int device, unsigned int output) const;
	unsigned int getViewportHeight(unsigned int device, unsigned int output) const;

	unsigned int getActiveViewportWidth(void);
	unsigned int getActiveViewportHeight(void);

	unsigned int getNumOutputs(unsigned int device) const;
	unsigned int getNumDevices(void) const;

	IRenderTargetPtr getOutputRenderTarget(unsigned int device, unsigned int output);
	IRenderTargetPtr getActiveOutputRenderTarget(void);

	bool setCurrentOutput(unsigned int output);
	unsigned int getCurrentOutput(void) const;

	bool setCurrentDevice(unsigned int device);
	unsigned int getCurrentDevice(void) const;

	unsigned int getDeviceForAdapter(unsigned int adapter_id) const;
	unsigned int getDeviceForMonitor(unsigned int monitor) const;

	IRenderDevice* createDeferredRenderDevice(void);
	void executeCommandList(ICommandList* command_list);
	bool finishCommandList(ICommandList* command_list);

	// For supporting deferred contexts
	void setRenderState(const RenderStateGL* render_state);

	void setLayout(LayoutGL* layout, const IMesh* mesh);
	void unsetLayout(LayoutGL* layout);

	void bindShader(ProgramGL* shader, ProgramBuffersGL* program_buffers);
	void unbindShader(void);

	void renderMeshNonIndexed(unsigned int topology, unsigned int vert_count, unsigned int start_location);
	void renderMeshInstanced(MeshGL* mesh, unsigned int count);
	void renderMesh(MeshGL* mesh);

private:
	struct OutputInfo
	{
		GleamArray<DEVMODE> display_mode_list;
		GleamAString name;
	};

	struct AdapterInfo
	{
		GleamArray<OutputInfo> output_info;
		DISPLAY_DEVICE display_device;
		GleamAString name;
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
		GleamArray<HGLRC> contexts;
		GleamArray<HDC> outputs;
		GleamArray<Viewport> viewports;
		GleamArray<HWND> windows;
		GleamArray<IRenderTargetPtr> rts;
		GleamBitArray vsync;

		unsigned int adapter_id;
	};

	GleamArray<AdapterInfo> _display_info;
	GleamArray<Device> _devices;

	// Key is thread id, value is 2D array. First index is device, second is output.
	GleamMap< unsigned int, GleamArray< GleamArray<HGLRC> > > _thread_contexts;
	Gaff::SpinLock _thread_data_lock;

	// Fix this
	static THREAD_LOCAL const Viewport* _active_viewport;
	static THREAD_LOCAL HDC _active_output;

	static THREAD_LOCAL unsigned int _curr_output;
	static THREAD_LOCAL unsigned int _curr_device;

	bool _glew_already_initialized;
};

NS_END
