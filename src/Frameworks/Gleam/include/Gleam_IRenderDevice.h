/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gleam_Vector.h"
#include "Gleam_Vec4.h"
#include "Gleam_Vec2.h"
#include <Gaff_Assert.h>

NS_GLEAM

class IRenderOutput;
class ICommandList;
//class IWindow;

class IRenderDevice
{
public:
	struct DisplayMode final
	{
		int32_t refresh_rate;
		int32_t width;
		int32_t height;
		int32_t id;
	};

	struct Display final
	{
		char display_name[128];
		Vector<DisplayMode> display_modes;
		int32_t id;

		int32_t curr_x;
		int32_t curr_y;
		int32_t curr_width;
		int32_t curr_height;

		bool is_primary;
	};

	struct Adapter final
	{
		char adapter_name[128];
		Vector<Display> displays;
		int32_t memory;
		int32_t id;
	};

	using AdapterList = Vector<Adapter>;

	IRenderDevice(void) {}
	virtual ~IRenderDevice(void) {}

	virtual bool init(const char* adapter_name) = 0;
	virtual bool init(int32_t adapter_id) = 0;

	//virtual bool resize(const IWindow& window) = 0;
	//virtual bool handleFocusGained(const IWindow& window) = 0;

	virtual IRenderDevice* getOwningDevice(void) const = 0;
	virtual bool isDeferred(void) const = 0;
	virtual RendererType getRendererType(void) const = 0;

	virtual IRenderDevice* createDeferredRenderDevice(void) = 0;
	virtual void executeCommandList(ICommandList& command_list) = 0;
	virtual bool finishCommandList(ICommandList& command_list) = 0;

	virtual void clearRenderState(void) = 0;
	virtual void renderLineNoVertexInputInstanced(int32_t instance_count) = 0;
	virtual void renderLineNoVertexInput(void) = 0;
	virtual void renderNoVertexInput(int32_t vert_count) = 0;

	virtual void setScissorRect(const IVec2& pos, const IVec2& size) = 0;
	virtual void setScissorRect(const IVec4& rect) = 0;

	virtual void* getUnderlyingDevice(void) = 0;

	int32_t getAdapterID(void) const { return _adapter_id; }

protected:
	int32_t _adapter_id = -1;

	GAFF_NO_COPY(IRenderDevice);
};

NS_END
