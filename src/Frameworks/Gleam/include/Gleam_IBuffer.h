/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include <Gaff_Defines.h>

NS_GLEAM

class IRenderDevice;

class IBuffer
{
public:
	enum BufferType {
		BT_VERTEX_DATA = 0,
		BT_INDEX_DATA,
		BT_SHADER_DATA,
		BT_STRUCTURED_DATA,
		
		BUFFER_TYPE_SIZE
	};

	enum MapType {
		MT_NONE = 0,
		MT_READ,
		MT_WRITE,
		MT_READ_WRITE,
		MT_WRITE_NO_OVERWRITE,

		MAP_TYPE_SIZE
	};

	template <class T>
	bool update(IRenderDevice& rd, const T* data, int32_t count)
	{
		return update(rd, (void*)data, sizeof(T) * count);
	}

	template <class T>
	bool update(IRenderDevice& rd, const T& data)
	{
		return update(rd, (void*)&data, sizeof(T));
	}

	struct BufferSettings
	{
		const void* data;
		size_t size;
		int32_t stride;
		BufferType type;
		MapType cpu_access;
		bool gpu_read_only;
		int32_t structure_byte_stride;
	};

	IBuffer(void) {}
	virtual ~IBuffer(void) {}

	bool init(IRenderDevice& rd, const BufferSettings& buffer_settings)
	{
		return init(
			rd, buffer_settings.data, buffer_settings.size, buffer_settings.type,
			buffer_settings.stride, buffer_settings.cpu_access,
			buffer_settings.gpu_read_only, buffer_settings.structure_byte_stride
		);
	}

	virtual bool init(
		IRenderDevice& rd, const void* data, size_t size, BufferType buffer_type = BT_SHADER_DATA,
		int32_t stride = 0, MapType cpu_access = MT_NONE, bool gpu_read_only = true,
		int32_t structure_byte_stride = 0
	) = 0;
	virtual void destroy(void) = 0;

	virtual bool update(IRenderDevice& rd, const void* data, size_t size, size_t offset = 0) = 0;
	virtual void* map(IRenderDevice& rd, MapType map_type = MT_WRITE) = 0;
	virtual void unmap(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;

	BufferType getBufferType(void) const { return _buffer_type; }
	int32_t getStructuredByteStride(void) const { return _structure_stride; }
	int32_t getStride(void) const { return _stride; }
	size_t getSize(void) const { return _size; }

protected:
	BufferType _buffer_type;
	int32_t _structure_stride;
	int32_t _stride;
	size_t _size;

	GAFF_NO_COPY(IBuffer);
};

NS_END
