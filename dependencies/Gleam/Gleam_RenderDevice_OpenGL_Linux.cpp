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

#include "Gleam_RenderDevice_OpenGL_Linux.h"
#include "Gleam_Window_Linux.h"
#include "Gleam_Global.h"

NS_GLEAM

RenderDeviceGL::RenderDeviceGL(void):
	_window(NULLPTR), _vsync(false), _context(0)
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

bool RenderDeviceGL::init(const Window& window, bool vsync)
{
	_context = glXCreateContext(window.getDisplay(), window.getVisualInfo(), NULL, True);

	if (!_context) {
		return false;
	}

	if (glXMakeCurrent(window.getDisplay(), window.getWindow(), _context) == False) {
		return false;
	}

	if (glewInit() != GLEW_OK) {
		return false;
	}

	if (GLXEW_EXT_swap_control) {
		glXSwapIntervalEXT(window.getDisplay(), glXGetCurrentDrawable(), (vsync) ? 1 : 0);
		_vsync = vsync;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	_window = &window;

	return true;
}

void RenderDeviceGL::destroy(void)
{
	if (_window && _context) {
		glXMakeCurrent(_window->getDisplay(), None, NULL);
		glXDestroyContext(_window->getDisplay(), _context);

		_window = NULLPTR;
		_context = 0;
	}
}

GLXContext RenderDeviceGL::getDeviceContext(void) const
{
	return _context;
}

bool RenderDeviceGL::isVsync(void) const
{
	return _vsync;
}

bool RenderDeviceGL::setVsync(bool vsync)
{
	if (GLXEW_EXT_swap_control) {
		_vsync = vsync;
		return glXSwapIntervalEXT(_window->getDisplay(), glXGetCurrentDrawable(), (vsync) ? 1 : 0);
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
	glXSwapBuffers(_window->getDisplay(), _window->getWindow());
}

bool RenderDeviceGL::resize(const Window& window)
{
	return true;
}

void RenderDeviceGL::unbindRenderTarget(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderDeviceGL::resetRenderState(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);
	glViewport(0, 0, window.getWidth(), window.getHeight());
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
