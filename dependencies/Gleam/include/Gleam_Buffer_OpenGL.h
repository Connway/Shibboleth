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

#include "Gleam_RefCounted.h"
#include "Gleam_IBuffer.h"

NS_GLEAM

class BufferGL : public IBuffer
{
public:
	BufferGL(void);
	~BufferGL(void);

	bool init(
		IRenderDevice&, const void* data, unsigned int size, BUFFER_TYPE buffer_type = SHADER_DATA,
		unsigned int stride = 0, MAP_TYPE cpu_access = NONE, bool gpu_read_only = true,
		unsigned int structure_byte_stride = 0
	);
	void destroy(void);

	bool update(IRenderDevice&, const void* data, unsigned int size, unsigned int offset = 0);
	void* map(IRenderDevice&, MAP_TYPE map_type = WRITE);
	void unmap(IRenderDevice&);

	bool isD3D(void) const;

	INLINE unsigned int getBuffer(void) const;

private:
	unsigned int _buffer;
	unsigned int _map_flags;

	GLEAM_REF_COUNTED(BufferGL);
};

NS_END
