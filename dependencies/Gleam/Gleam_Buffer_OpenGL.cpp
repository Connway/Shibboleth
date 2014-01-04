/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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
#include <Gaff_IncludeAssert.h>
#include <GL/glew.h>

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

BufferGL::BufferGL(void):
	_buffer(0)
{
}

BufferGL::~BufferGL(void)
{
	destroy();
}

bool BufferGL::init(const IRenderDevice&, const void* data, unsigned int size, BUFFER_TYPE buffer_type, unsigned int stride, MAP_TYPE cpu_access)
{
	assert(!_buffer);

	GLenum buff_type = _type_map[buffer_type];

	glGenBuffers(1, &_buffer);
	glBindBuffer(buff_type, _buffer);
	glBufferData(buff_type, size, data, (cpu_access != NONE) ? GL_STREAM_DRAW : GL_STATIC_DRAW);
	glBindBuffer(buff_type, 0);

	_buffer_type = buffer_type;
	_stride = stride;
	_size = size;

	return glGetError() == GL_NO_ERROR;
}

void BufferGL::destroy(void)
{
	if (_buffer) {
		glDeleteBuffers(1, &_buffer);
		_buffer = 0;
	}
}

bool BufferGL::update(const IRenderDevice&, const void* data, unsigned int size)
{
	assert(data);
	glBufferSubData(_buffer, 0, size, data);
	return true;
}

void* BufferGL::map(const IRenderDevice&, MAP_TYPE map_type)
{
	assert(map_type != NONE);
	GLenum buff_type = _type_map[_buffer_type];
	glBindBuffer(buff_type, _buffer);
	return glMapBuffer(buff_type, _map_map[map_type - 1]);
}

void BufferGL::unmap(const IRenderDevice&)
{
	glUnmapBuffer(_type_map[_buffer_type]);
}

bool BufferGL::isD3D(void) const
{
	return false;
}

unsigned int BufferGL::getBuffer(void) const
{
	return _buffer;
}

NS_END
