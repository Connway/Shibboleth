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

#include "Gleam_IRenderDevice.h"

NS_GLEAM

class DeferredRenderDeviceBase : public IRenderDevice
{
public:
	DeferredRenderDeviceBase(void);
	~DeferredRenderDeviceBase(void);

	AdapterList getDisplayModes(int compat = 28);

	bool initThreadData(unsigned int* thread_ids, size_t num_ids);
	bool init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync = false);

	bool isVsync(unsigned int device, unsigned int output) const;
	void setVsync(bool vsync, unsigned int device, unsigned int output);

	void setClearColor(float r, float g, float b, float a);

	void beginFrame(void);
	void endFrame(void);

	bool resize(const IWindow& window);
	bool handleFocusGained(const IWindow& window);

	void resetRenderState(void);

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
};

NS_END
