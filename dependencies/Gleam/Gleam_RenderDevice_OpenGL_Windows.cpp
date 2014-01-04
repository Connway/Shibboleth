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

#include "Gleam_RenderDevice_OpenGL_Windows.h"
#include "Gleam_Window_Windows.h"
#include "Gleam_Buffer_OpenGL.h"
#include <Gaff_IncludeAssert.h>
#include <GL/glew.h>
#include <GL/wglew.h>

NS_GLEAM

RenderDeviceGL::RenderDeviceGL(void):
	_vsync(false), _context(NULLPTR), _hwnd(NULLPTR), _hdc(NULLPTR)
{
}

RenderDeviceGL::~RenderDeviceGL(void)
{
	destroy();
}

bool RenderDeviceGL::CheckRequiredExtensions(void)
{
	return GLEW_VERSION_4_3 ||
	(
		GLEW_ARB_compute_shader && GLEW_ARB_geometry_shader4 &&
		GLEW_ARB_tessellation_shader && GLEW_ARB_explicit_uniform_location &&
		GLEW_ARB_sampler_objects && GLEW_ARB_separate_shader_objects &&
		GLEW_EXT_texture_filter_anisotropic && GLEW_ARB_shading_language_420pack &&
		GLEW_ARB_vertex_array_object
		//GLEW_ARB_shader_image_load_store
	);
}

bool RenderDeviceGL::init(const Window& window, bool vsync, int, unsigned int)
{
	_hwnd = window.getHWnd();
	_hdc = GetDC(_hwnd);

	if (!_hdc) {
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd, *ppfd;
	int pixelformat;

	ppfd = &pfd;

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->nVersion = 1;
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ppfd->dwLayerMask = PFD_MAIN_PLANE;
	ppfd->iPixelType = PFD_TYPE_COLORINDEX;
	ppfd->cColorBits = 8;
	ppfd->cDepthBits = 16;
	ppfd->cAccumBits = 0;
	ppfd->cStencilBits = 0;

	pixelformat = ChoosePixelFormat(_hdc, ppfd);

	if (!pixelformat) {
		return false;
	}

	if (!SetPixelFormat(_hdc, pixelformat, ppfd)) {
		return false;
	} 

	_context = wglCreateContext(_hdc);

	if (!_context) {
		return false;
	}

	if (!wglMakeCurrent(_hdc, _context)) {
		return false;
	}

	if (glewInit() != GLEW_OK) {
		return false;
	}

	if (WGLEW_EXT_swap_control) {
		wglSwapIntervalEXT((vsync) ? 1 : 0);
		_vsync = vsync;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	_width = window.getWidth();
	_height = window.getHeight();

	return true;
}

void RenderDeviceGL::destroy(void)
{
	if (_context) {
		wglDeleteContext(_context);
		_context = NULLPTR;
	}

	if (_hdc) {
		wglMakeCurrent(_hdc, NULL);
		ReleaseDC(_hwnd, _hdc);
		_hdc = NULLPTR;
	}

	_hwnd = NULLPTR;
}

HGLRC RenderDeviceGL::getDeviceContext(void) const
{
	return _context;
}

bool RenderDeviceGL::isVsync(void) const
{
	return _vsync;
}

bool RenderDeviceGL::setVsync(bool vsync)
{
	if (WGLEW_EXT_swap_control) {
		_vsync = vsync;
		return wglSwapIntervalEXT((vsync) ? 1 : 0) == TRUE;
	}

	return false;
}

void RenderDeviceGL::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RenderDeviceGL::beginFrame(void)
{
	resetRenderState();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
}

void RenderDeviceGL::endFrame(void)
{
	SwapBuffers(_hdc);
}

bool RenderDeviceGL::resize(const Window& window)
{
	_width = window.getWidth();
	_height = window.getHeight();
	return true;
}

void RenderDeviceGL::unbindRenderTarget(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	_prev_rt = NULLPTR;
}

void RenderDeviceGL::resetRenderState(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glDisable(GL_BLEND);
	glViewport(0, 0, _width, _height);
}

bool RenderDeviceGL::isD3D(void) const
{
	return false;
}

unsigned int RenderDeviceGL::getViewportWidth(void) const
{
	return _width;
}

unsigned int RenderDeviceGL::getViewportHeight(void) const
{
	return _height;
}

NS_END
