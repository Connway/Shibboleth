/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
	enum class Type
	{
		VertexData = 0,
		IndexData,
		ShaderConstantData,
		StructuredData,

		Count
	};

	enum class MapType
	{
		None = 0,
		Read,
		Write,
		ReadWrite,
		WriteNoOverwrite,

		Count
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

	struct Settings final
	{
		const void* data = nullptr;
		size_t size = 0;
		int32_t stride = 0;
		int32_t element_size = 0;
		Type type = Type::ShaderConstantData;
		MapType cpu_access = MapType::None;
		bool gpu_read_only = true;
	};

	IBuffer(void) {}
	virtual ~IBuffer(void) {}

	virtual bool init(IRenderDevice& rd, const Settings& buffer_settings) = 0;
	virtual void destroy(void) = 0;

	virtual bool update(IRenderDevice& rd, const void* data, size_t size, size_t offset = 0) = 0;
	virtual void* map(IRenderDevice& rd, MapType map_type = MapType::Write) = 0;
	virtual void unmap(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;

	Type getBufferType(void) const { return _buffer_type; }
	int32_t getElementSize(void) const { return _elem_size; }
	int32_t getStride(void) const { return _stride; }
	size_t getSize(void) const { return _size; }

protected:
	Type _buffer_type;
	int32_t _elem_size;
	int32_t _stride;
	size_t _size;

	GAFF_NO_COPY(IBuffer);
};

NS_END
