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

#ifdef __linux__

#include "Gleam_RenderDevice_OpenGL_Linux.h"
#include "Gleam_RenderTarget_OpenGL.h"
#include "Gleam_Window_Linux.h"
#include "Gleam_Global.h"
#include <cstdio>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#define NUM_DEVICES_OUTPUTS_TO_ITERATE 16

NS_GLEAM

RenderDeviceGL::RenderDeviceGL(void):
	_active_viewport(nullptr), _active_window(nullptr),
	_curr_output(0), _curr_device(0),
	_glew_already_initialized(false)
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
	);
}

IRenderDevice::AdapterList RenderDeviceGL::getDisplayModes(int)
{
	// Since, to my knowledge, there's no way of querying how many X Servers
	// and X Displays there are
	for (unsigned int i = 0; i < NUM_DEVICES_OUTPUTS_TO_ITERATE; ++i) {
		AdapterInfo adapter;

		for (unsigned int j = 0; j < NUM_DEVICES_OUTPUTS_TO_ITERATE; ++j) {
			char buff[8] = { 0 };
			sprintf(buff, ":%i.%i", i, j);

			::Display* display = XOpenDisplay(buff);

			// We didn't open, we've probably reached the number of monitors attached to the device
			if (!display) {
				break;
			}

			OutputInfo output;
			output.name = buff;

			// Grab display modes
			::Window root = DefaultRootWindow(display);

			if (!root) {
				XCloseDisplay(display);
				return AdapterList();
			}

			int final_size = 0, num_sizes = 0, num_rates = 0;
			short* rates = nullptr;
			XRRScreenConfiguration* config = XRRGetScreenInfo(display, root);
			XRRScreenSize* sizes = XRRConfigSizes(config, &num_sizes);

			if (!config || !sizes || num_sizes < 1) {
				if (config) {
					XRRFreeScreenConfigInfo(config);
				}

				XCloseDisplay(display);
				return AdapterList();
			}

			for (int k = 0; k < num_sizes; ++k) {
				rates = XRRConfigRates(config, final_size, &num_rates);

				if (!rates || num_rates < 1) {
					XRRFreeScreenConfigInfo(config);
					XCloseDisplay(display);
					return AdapterList();
				}

				ScreenMode mode = { sizes[k].width, sizes[k].height, 0 };

				for (int l = 0; l < num_rates; ++l) {
					mode.refresh_rate = rates[l];
					output.display_mode_list.push(mode);
				}
			}

			adapter.output_info.push(output);

			XCloseDisplay(display);
		}

		_display_info.push(adapter);
	}

	AdapterList out(_display_info.size());

	for (unsigned int i = 0; i < _display_info.size(); ++i) {
		const AdapterInfo& adpt_info = _display_info[i];

		Adapter adpt;
		adpt.displays.reserve(adpt_info.output_info.size());
		adpt.id = i;

		for (unsigned int j = 0; j < adpt_info.output_info.size(); ++j) {
			const OutputInfo& out_info = adpt_info.output_info[j];
			Display display;
			display.display_modes.reserve(out_info.display_mode_list.size());
			display.id = j;

			for (unsigned int k = 0; k < out_info.display_mode_list.size(); ++k) {
				const ScreenMode& mode_desc = out_info.display_mode_list[k];

				DisplayMode mode = {
					(unsigned int)mode_desc.refresh_rate,
					(unsigned int)mode_desc.width,
					(unsigned int)mode_desc.height,
					k, 0, 0
				};

				display.display_modes.push(mode);
			}

			adpt.displays.push(display);
		}

		out.push(adpt);
	}

	return out;
}

bool RenderDeviceGL::init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync)
{
	assert(
		_display_info.size() > adapter_id &&
		_display_info[adapter_id].output_info.size() > display_id &&
		_display_info[adapter_id].output_info[display_id].display_mode_list.size() > display_mode_id
	);

	const Window& wnd = (const Window&)window;

	GleamArray<Device>::Iterator it = _devices.linearSearch(adapter_id, [](const Device& lhs, unsigned int rhs) -> bool
	{
		return lhs.adapter_id == rhs;
	});

	GLXContext context = nullptr;

	if (it == _devices.end()) {
		context = glXCreateContext(wnd.getDisplay(), wnd.getVisualInfo(), 0, True);

		if (!context) {
			return false;
		}

		Device device;

		Viewport viewport = { 0, 0, (int)wnd.getWidth(), (int)wnd.getHeight() };

		device.contexts.push(context);
		device.viewports.push(viewport);
		device.windows.push((Window*)&window);
		device.vsync.push(vsync);
		device.adapter_id = adapter_id;

		RenderTargetGL* rt = GleamAllocateT(RenderTargetGL);
		rt->setViewport(viewport.width, viewport.height);

		device.rts.push(IRenderTargetPtr(rt));

		_devices.push(device);

	} else {
		// Create the context with a share list
		context = glXCreateContext(wnd.getDisplay(), wnd.getVisualInfo(), it->contexts[0], True);

		if (!context) {
			return false;
		}

		Viewport viewport = { 0, 0, (int)wnd.getWidth(), (int)wnd.getHeight() };
		it->contexts.push(context);
		it->viewports.push(viewport);
		it->windows.push((Window*)&window);
		it->vsync.push(vsync);

		RenderTargetGL* rt = GleamAllocateT(RenderTargetGL);
		rt->setViewport(viewport.width, viewport.height);

		it->rts.push(IRenderTargetPtr(rt));
	}

	if (!_glew_already_initialized) {
		if (glXMakeCurrent(wnd.getDisplay(), wnd.getWindow(), context) == False) {
			return false;
		}
		
		if (glewInit() != GLEW_OK) {
			return false;
		}

		if (GLXEW_EXT_swap_control) {
			glXSwapIntervalEXT(wnd.getDisplay(), glXGetCurrentDrawable(), vsync);
		}

		_glew_already_initialized = true;
		setCurrentDevice(0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	return true;
}

void RenderDeviceGL::destroy(void)
{
	for (auto it = _devices.begin(); it != _devices.end(); ++it) {
		for (unsigned int i = 0; i < it->windows.size(); ++i) {
			glXMakeCurrent(it->windows[i]->getDisplay(), None, 0);
			glXDestroyContext(it->windows[i]->getDisplay(), it->contexts[i]);
		}
	}

	_devices.clear();
}

bool RenderDeviceGL::isVsync(unsigned int device, unsigned int output) const
{
	assert(_devices.size() > device && _devices[device].vsync.size() > output);
	return _devices[device].vsync[output];
}

void RenderDeviceGL::setVsync(bool vsync, unsigned int device, unsigned int output)
{
	assert(_devices.size() > device && _devices[device].vsync.size() > output);
	_devices[device].vsync.setBit(output, vsync);
}

void RenderDeviceGL::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RenderDeviceGL::beginFrame(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].windows.size() > _curr_output);
	resetRenderState();
	glViewport(_active_viewport->x, _active_viewport->y, _active_viewport->width, _active_viewport->height);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderDeviceGL::endFrame(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].windows.size() > _curr_output);
	glXSwapBuffers(_active_window->getDisplay(), _active_window->getWindow());
}

bool RenderDeviceGL::resize(const IWindow& window)
{
	const Window& wnd = (const Window&)window;

	for (auto it = _devices.begin(); it != _devices.end(); ++it) {
		int index = it->windows.linearSearch(0, it->windows.size(), &window);

		if (index > -1) {
			it->viewports[index].width = wnd.getWidth();
			it->viewports[index].height = wnd.getHeight();
		}

		if (&window == _active_window) {
			glViewport(0, 0, wnd.getWidth(), wnd.getHeight());
		}

		// Maybe detect and handle fullscreen changes here?
	}

	return true;
}

bool RenderDeviceGL::handleFocusGained(const IWindow&)
{
	return true;
}

void RenderDeviceGL::resetRenderState(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glDisable(GL_BLEND);
}

bool RenderDeviceGL::isD3D(void) const
{
	return false;
}

unsigned int RenderDeviceGL::getViewportWidth(unsigned int device, unsigned int output) const
{
	assert(_devices.size() > device && _devices[device].viewports.size() > output);
	return (unsigned int)_devices[device].viewports[output].width;
}

unsigned int RenderDeviceGL::getViewportHeight(unsigned int device, unsigned int output) const
{
	assert(_devices.size() > device && _devices[device].viewports.size() > output);
	return (unsigned int)_devices[device].viewports[output].height;
}

unsigned int RenderDeviceGL::getActiveViewportWidth(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);
	return getViewportWidth(_curr_device, _curr_output);
}

unsigned int RenderDeviceGL::getActiveViewportHeight(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);
	return getViewportHeight(_curr_device, _curr_output);
}

unsigned int RenderDeviceGL::getNumOutputs(unsigned int device) const
{
	assert(_devices.size() > device);
	return _devices[device].windows.size();
}

unsigned int RenderDeviceGL::getNumDevices(void) const
{
	return _devices.size();
}

IRenderTargetPtr RenderDeviceGL::getOutputRenderTarget(unsigned int device, unsigned int output)
{
	assert(_devices.size() > device && _devices[device].rts.size() > output);
	return _devices[device].rts[output];
}

IRenderTargetPtr RenderDeviceGL::getActiveOutputRenderTarget(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].rts.size() > _curr_output);
	return getOutputRenderTarget(_curr_device, _curr_output);
}

bool RenderDeviceGL::setCurrentOutput(unsigned int output)
{
	assert(_devices[_curr_device].windows.size() > output);

	Window* new_window = _devices[_curr_device].windows[output];

	if (glXMakeCurrent(new_window->getDisplay(), new_window->getWindow(), _devices[_curr_device].contexts[output]) == False) {
		return false;
	}

	if (GLXEW_EXT_swap_control) {
		glXSwapIntervalEXT(new_window->getDisplay(), glXGetCurrentDrawable(), _devices[_curr_device].vsync[0]);
	}

	_active_viewport = &_devices[_curr_device].viewports[output];
	_active_window = new_window;
	_curr_output = output;

	return true;
}

unsigned int RenderDeviceGL::getCurrentOutput(void) const
{
	return _curr_output;
}

bool RenderDeviceGL::setCurrentDevice(unsigned int device)
{
	assert(_devices.size() > device && _devices[device].vsync.size() > 0);
	unsigned int prev_device = _curr_device;
	_curr_device = device;

	if (!setCurrentOutput(0)) {
		_curr_device = prev_device;
		setCurrentOutput(_curr_output); // attempt to set back to the old device
		return false;
	}

	return true;
}

unsigned int RenderDeviceGL::getCurrentDevice(void) const
{
	return _curr_device;
}

int RenderDeviceGL::getDeviceForAdapter(unsigned int adapter_id) const
{
	for (unsigned int i = 0; i < _devices.size(); ++i) {
		if (_devices[i].adapter_id == adapter_id) {
			return i;
		}
	}

	return -1;
}

NS_END

#endif