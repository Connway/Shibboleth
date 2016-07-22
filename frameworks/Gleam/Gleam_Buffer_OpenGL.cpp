/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gleam_Buffer_OpenGL.h"
#include <Gaff_Assert.h>
#include <GL/glew.h>
#include <cstring>

NS_GLEAM

static GLenum _type_map[IBuffer::BUFFER_TYPE_SIZE] = {
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_UNIFORM_BUFFER
};

static GLenum _map_map[IBuffer::MAP_TYPE_SIZE] = {
	0,
	GL_READ_ONLY,
	GL_WRITE_ONLY,
	GL_READ_WRITE
};

static GLbitfield _map_bit_flags[IBuffer::MAP_TYPE_SIZE] = {
	0,
	GL_MAP_READ_BIT,
	GL_MAP_WRITE_BIT,
	GL_MAP_READ_BIT | GL_MAP_WRITE_BIT,
	GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT
};

BufferGL::BufferGL(void):
	_buffer(0), _map_flags(0)
{
}

BufferGL::~BufferGL(void)
{
	destroy();
}

bool BufferGL::init(
	IRenderDevice&, const void* data, unsigned int size, BUFFER_TYPE buffer_type,
	unsigned int stride, MAP_TYPE cpu_access, bool gpu_read_only, unsigned int structure_byte_stride)
{
	GAFF_ASSERT(!_buffer);

	GLenum buff_type = _type_map[buffer_type];

	glGenBuffers(1, &_buffer);

	glBindBuffer(buff_type, _buffer);
	glBufferData(buff_type, size, data, (gpu_read_only && (cpu_access == WRITE || cpu_access == WRITE_NO_OVERWRITE)) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBuffer(buff_type, 0);

	_map_flags = _map_bit_flags[cpu_access];
	_buffer_type = buffer_type;
	_structure_stride = structure_byte_stride;
	_stride = stride;
	_size = size;

	glFinish();

	return glGetError() == GL_NO_ERROR;
}

void BufferGL::destroy(void)
{
	if (_buffer) {
		glDeleteBuffers(1, &_buffer);
		_buffer = 0;
	}
}

bool BufferGL::update(IRenderDevice& rd, const void* data, unsigned int size, unsigned int offset)
{
	GAFF_ASSERT(data && size);

	GLenum buff_type = _type_map[_buffer_type];
	glBindBuffer(buff_type, _buffer);

	// This isn't updating the buffer. :/
	//glBufferSubData(buff_type, offset, size, data);
	//return glGetError()  == GL_NO_ERROR;

	void* buffer = map(rd, (offset) ? WRITE_NO_OVERWRITE : WRITE);

	if (!buffer)
	{
		return false;
	}

	memcpy(reinterpret_cast<char*>(buffer) + offset, data, size);
	unmap(rd);

	return true;
}

void* BufferGL::map(IRenderDevice&, MAP_TYPE map_type)
{
	GAFF_ASSERT(map_type != NONE);
	GLenum buff_type = _type_map[_buffer_type];
	glBindBuffer(buff_type, _buffer);
	return glMapBufferRange(buff_type, 0, _size, _map_bit_flags[map_type]);
}

void BufferGL::unmap(IRenderDevice&)
{
	glUnmapBuffer(_type_map[_buffer_type]);
}

RendererType BufferGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

unsigned int BufferGL::getBuffer(void) const
{
	return _buffer;
}

NS_END
