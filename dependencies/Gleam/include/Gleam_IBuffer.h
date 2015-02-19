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

#include "Gleam_Defines.h"
#include <Gaff_IRefCounted.h>

NS_GLEAM

class IRenderDevice;

class IBuffer : public Gaff::IRefCounted
{
public:
	enum BUFFER_TYPE {
		VERTEX_DATA = 0,
		INDEX_DATA,
		SHADER_DATA,
		STRUCTURED_DATA,
		BUFFER_TYPE_SIZE
	};

	enum MAP_TYPE {
		NONE = 0,
		READ,
		WRITE,
		READ_WRITE,
		WRITE_NO_OVERWRITE,
		MAP_TYPE_SIZE
	};

	template <class T>
	bool update(IRenderDevice& rd, const T* data, unsigned int count)
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
		unsigned int size;
		unsigned int stride;
		BUFFER_TYPE type;
		MAP_TYPE cpu_access;
		bool gpu_read_only;
	};

	IBuffer(void) {}
	virtual ~IBuffer(void) {}

	INLINE bool init(IRenderDevice& rd, const BufferSettings& buffer_settings)
	{
		return init(rd, buffer_settings.data, buffer_settings.size, buffer_settings.type, buffer_settings.stride, buffer_settings.cpu_access, buffer_settings.gpu_read_only);
	}

	virtual bool init(IRenderDevice& rd, const void* data, unsigned int size, BUFFER_TYPE buffer_type = SHADER_DATA,
						unsigned int stride = 0, MAP_TYPE cpu_access = NONE, bool gpu_read_only = true) = 0;
	virtual void destroy(void) = 0;

	virtual bool update(IRenderDevice& rd, const void* data, unsigned int size, unsigned int offset = 0) = 0;
	virtual void* map(IRenderDevice& rd, MAP_TYPE map_type = WRITE) = 0;
	virtual void unmap(IRenderDevice& rd) = 0;

	virtual bool isD3D(void) const = 0;

	INLINE BUFFER_TYPE getBufferType(void) const { return _buffer_type; }
	INLINE unsigned int getStride(void) const { return _stride; }
	INLINE unsigned int getSize(void) const { return _size; }

protected:
	BUFFER_TYPE _buffer_type;
	unsigned int _stride;
	unsigned int _size;

	GAFF_NO_COPY(IBuffer);
};

NS_END
