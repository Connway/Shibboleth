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

#include "Shibboleth_GraphicsDefines.h"
#include <Gleam_CommandList.h>

NS_GLEAM
	class RenderDevice;
NS_END


NS_SHIBBOLETH

struct RenderCommands final
{
	UniquePtr<Gleam::CommandList> commands; // $TODO: This is basically a unique ptr to a ref ptr. Remove the unique ptr?
	//Gleam::RenderTarget* target = nullptr;
};

struct RenderCommandList final
{
	Vector<RenderCommands> command_list{ GRAPHICS_ALLOCATOR };

	// $TODO: Evaluate if we need this once converted over to render pipeline.
	//EA::Thread::SpinLock lock;
};

struct RenderCommandData final
{
	using DeviceCommandListMap = VectorMap<const Gleam::RenderDevice*, RenderCommandList>;
	static constexpr int32_t CacheIndexCount = 2;

	DeviceCommandListMap command_lists[CacheIndexCount] =
	{
		DeviceCommandListMap{ GRAPHICS_ALLOCATOR },
		DeviceCommandListMap{ GRAPHICS_ALLOCATOR }
	};

	const RenderCommandList& getCommandList(const Gleam::RenderDevice& device, int32_t cache_index) const
	{
		return const_cast<RenderCommandData*>(this)->getCommandList(device, cache_index);
	}

	RenderCommandList& getCommandList(const Gleam::RenderDevice& device, int32_t cache_index)
	{
		return command_lists[cache_index][&device];
	}
};

NS_END
