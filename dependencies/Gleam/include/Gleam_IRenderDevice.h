/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_IRenderTarget.h"
#include "Gleam_Array.h"

NS_GLEAM

class IWindow;

class IRenderDevice
{
public:
	struct DisplayMode
	{
		unsigned int refresh_rate;
		unsigned int width;
		unsigned int height;
		unsigned int id;
		int x;
		int y;
	};
	
	struct Display
	{
		GleamArray<DisplayMode> display_modes;
		unsigned int id;
	};

	struct Adapter
	{
		char adapter_name[128];
		GleamArray<Display> displays;
		unsigned int memory;
		unsigned int id;
	};

	typedef GleamArray<Adapter> AdapterList;

	IRenderDevice(void) {}
	virtual ~IRenderDevice(void) {}

	virtual AdapterList getDisplayModes(int compat = 28) = 0;

	virtual bool init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync = false) = 0;
	virtual void destroy(void) = 0;

	virtual bool isVsync(unsigned int device, unsigned int output) const = 0;
	virtual void setVsync(bool vsync, unsigned int device, unsigned int output) = 0;

	virtual void setClearColor(float r, float g, float b, float a) = 0;

	virtual void beginFrame(void) = 0;
	virtual void endFrame(void) = 0;

	virtual bool resize(const IWindow& window) = 0;
	virtual bool handleFocusGained(const IWindow& window) = 0;

	virtual void resetRenderState(void) = 0;

	virtual bool isD3D(void) const = 0;

	virtual unsigned int getViewportWidth(unsigned int device, unsigned int output) const = 0;
	virtual unsigned int getViewportHeight(unsigned int device, unsigned int output) const = 0;

	virtual unsigned int getActiveViewportWidth(void) = 0;
	virtual unsigned int getActiveViewportHeight(void) = 0;

	virtual unsigned int getNumOutputs(unsigned int device) const = 0;
	virtual unsigned int getNumDevices(void) const = 0;

	virtual IRenderTargetPtr getOutputRenderTarget(unsigned int device, unsigned int output) = 0;
	virtual IRenderTargetPtr getActiveOutputRenderTarget(void) = 0;

	virtual bool setCurrentOutput(unsigned int output) = 0;
	virtual unsigned int getCurrentOutput(void) const = 0;

	virtual bool setCurrentDevice(unsigned int device) = 0;
	virtual unsigned int getCurrentDevice(void) const = 0;

	virtual int getDeviceForAdapter(unsigned int adapter_id) const = 0;

	GAFF_NO_COPY(IRenderDevice);
};

NS_END
