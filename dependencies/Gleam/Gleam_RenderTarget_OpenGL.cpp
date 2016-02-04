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

#include "Gleam_RenderTarget_OpenGL.h"
#include "Gleam_Texture_OpenGL.h"
#include "Gleam_IRenderDevice.h"
#include "Gaff_IncludeAssert.h"
#include <GL/glew.h>

NS_GLEAM

RenderTargetGL::RenderTargetGL(void):
#ifdef OPENGL_MULTITHREAD
	_depth_stencil_texture(nullptr),
#endif
	_frame_buffer(0), _attach_count(0),
	_viewport_width(0), _viewport_height(0)
{
}

RenderTargetGL::~RenderTargetGL(void)
{
	destroy();
}

bool RenderTargetGL::init(void)
{
	assert(!_frame_buffer);

#ifndef OPENGL_MULTITHREAD
	glGenFramebuffers(1, &_frame_buffer);
	return _frame_buffer != 0;
#else
	return true;
#endif
}

void RenderTargetGL::destroy(void)
{
	if (_frame_buffer) {
		glDeleteFramebuffers(1, &_frame_buffer);
	}
}

bool RenderTargetGL::addTexture(IRenderDevice&, const ITexture* color_texture, CUBE_FACE face)
{
#ifdef _DEBUG
	{
		GLint max_color_attachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
		assert(_attach_count < GL_MAX_COLOR_ATTACHMENTS);
	}
#endif

	assert(color_texture && color_texture->getRendererType() == RENDERER_OPENGL);

#ifdef OPENGL_MULTITHREAD
	if (!_frame_buffer) {
		_color_textures.push(Gaff::MakePair(color_texture, face));
		return true;
	}
#endif

	GLint fb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);

	glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

	GLuint target = GL_TEXTURE_2D;

	if (face != NONE) {
		target = face + GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _attach_count, target, static_cast<const TextureGL*>(color_texture)->getTexture(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(fb));

	if (!_attach_count) {
		_viewport_width = color_texture->getWidth();
		_viewport_height = color_texture->getHeight();
	}

	if (glGetError() == GL_NO_ERROR) {
		_draw_buffers.push(GL_COLOR_ATTACHMENT0 + _attach_count);
		++_attach_count;
		return true;
	}

	return false;
}

void RenderTargetGL::popTexture(void)
{
#ifdef OPENGL_MULTITHREAD
	if (!_frame_buffer && !_color_textures.empty()) {
		_color_textures.pop();
		return;
	}
#endif

	assert(_attach_count > 0);
	_draw_buffers.pop();
	--_attach_count;
}

bool RenderTargetGL::addDepthStencilBuffer(IRenderDevice&, const ITexture* depth_stencil_texture)
{
	assert(depth_stencil_texture && depth_stencil_texture->getRendererType() == RENDERER_OPENGL);

#ifdef OPENGL_MULTITHREAD
	if (!_frame_buffer) {
		_depth_stencil_texture = depth_stencil_texture;
		return true;
	}
#endif

	GLint fb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);

	glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

	GLenum attachment = 0;

	switch (depth_stencil_texture->getType()) {
		case ITexture::DEPTH:
			attachment = GL_DEPTH_ATTACHMENT;
			break;

		case ITexture::DEPTH_STENCIL:
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;

		// To get GCC to stop erroring
		default:
			break;
	}

	assert(attachment != 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, static_cast<const TextureGL*>(depth_stencil_texture)->getTexture(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(fb));

	return glGetError() == GL_NO_ERROR;
}

void RenderTargetGL::bind(IRenderDevice& rd)
{
#ifdef OPENGL_MULTITHREAD
	if (!_frame_buffer && (!_color_textures.empty() || _depth_stencil_texture)) {
		glGenFramebuffers(1, &_frame_buffer);

		if (_frame_buffer) {
			for (auto it = _color_textures.begin(); it != _color_textures.end(); ++it) {
				addTexture(rd, it->first, it->second);
			}

			if (_depth_stencil_texture) {
				addDepthStencilBuffer(rd, _depth_stencil_texture);
			}

			_color_textures.clear();
		}
	}
#endif

	if (_frame_buffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
		glDrawBuffers(static_cast<unsigned int>(_draw_buffers.size()), _draw_buffers.getArray());
		glViewport(0, 0, _viewport_width, _viewport_height);

	} else {
		glViewport(0, 0, _viewport_width, _viewport_height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void RenderTargetGL::unbind(IRenderDevice&)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetGL::clear(IRenderDevice& rd, unsigned int clear_flags, float clear_depth, unsigned char clear_stencil, float* clear_color)
{
	assert(rd.getRendererType() == RENDERER_OPENGL);

	GLbitfield clear_bits = 0;

	if (clear_flags & CLEAR_COLOR) {
		clear_bits |= GL_COLOR_BUFFER_BIT;
	}

	if (clear_flags & CLEAR_DEPTH) {
		clear_bits |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(clear_depth);
	}

	if (clear_flags & CLEAR_STENCIL) {
		clear_bits |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(clear_stencil);
	}

	if (clear_color) {
		glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	}

	glClear(clear_bits); 
}

bool RenderTargetGL::isComplete(void) const
{
	if (!_frame_buffer) {
		return false;
	}

	GLint fb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

	bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(fb));

	return complete;
}

RendererType RenderTargetGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

void RenderTargetGL::setViewport(int viewport_width, int viewport_height)
{
	_viewport_width = viewport_width;
	_viewport_height = viewport_height;
}

NS_END
