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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_RenderDevice_OpenGL_Windows.h"
#include "Gleam_RenderTarget_OpenGL.h"
#include "Gleam_Window_Windows.h"

#include <Gaff_Thread.h>
#include <GL/glew.h>
#include <GL/wglew.h>

NS_GLEAM

const RenderDeviceGL::Viewport* RenderDeviceGL::_active_viewport = nullptr;
HDC RenderDeviceGL::_active_output = nullptr;

unsigned int RenderDeviceGL::_curr_output = UINT_FAIL;
unsigned int RenderDeviceGL::_curr_device = UINT_FAIL;


RenderDeviceGL::RenderDeviceGL(void):
	_glew_already_initialized(false)
{
}

RenderDeviceGL::~RenderDeviceGL(void)
{
	destroy();
}

// Assumed to be called in the main thread.
bool RenderDeviceGL::initThreadData(unsigned int* thread_ids, size_t num_ids)
{
	for (size_t i = 0; i < num_ids; ++i) {
		GleamArray< GleamArray<HGLRC> >& thread_data = _thread_contexts[thread_ids[i]];

		thread_data.resize(_devices.size());

		for (size_t j = 0; j < _devices.size(); ++j) {
			unsigned int num_outputs = getNumOutputs(static_cast<unsigned int>(j));
			thread_data[j].reserve(num_outputs);

			for (size_t k = 0; k < num_outputs; ++k) {
				HGLRC context = wglCreateContext(_devices[j].outputs[k]);

				if (!context) {
					return false;
				}

				if (wglShareLists(_devices[j].contexts[k], context) == FALSE) {
					wglDeleteContext(context);
					return false;
				}

				thread_data[j].push(context);
			}
		}
	}

	// Add main thread data
	GleamArray< GleamArray<HGLRC> >& thread_data = _thread_contexts[Gaff::Thread::GetCurrentThreadID()];
	GAFF_ASSERT(thread_data.empty());
	thread_data.resize(_devices.size());

	for (size_t j = 0; j < _devices.size(); ++j) {
		for (size_t k = 0; k < getNumOutputs(static_cast<unsigned int>(j)); ++k) {
			thread_data[j].push(_devices[j].contexts[k]);
		}
	}

	return true;
}

IRenderDevice::AdapterList RenderDeviceGL::getDisplayModes(int)
{
	DISPLAY_DEVICE disp_device;
	DEVMODE mode = {};

	disp_device.cb = sizeof(DISPLAY_DEVICE);

	for (unsigned int i = 0; EnumDisplayDevices(nullptr, i, &disp_device, EDD_GET_DEVICE_INTERFACE_NAME); ++i) {
		if (!(disp_device.StateFlags & DISPLAY_DEVICE_ACTIVE)) {
			continue;
		}

#ifdef _UNICODE
		GleamAString dev_string, dev_name;
		dev_string.convertToUTF8(disp_device.DeviceString, wcslen(disp_device.DeviceString));
		dev_name.convertToUTF8(disp_device.DeviceName, wcslen(disp_device.DeviceName));
#endif

		// Find an entry that already exists. We might be a different display on the same device.
		GleamArray<AdapterInfo>::Iterator it_disp = _display_info.linearSearch(dev_string.getBuffer(), [](const AdapterInfo& lhs, const char* rhs) -> bool
		{
			return lhs.name == rhs;
		});

		if (it_disp == _display_info.end()) {
			AdapterInfo adapter;
			adapter.display_device = disp_device;
#ifdef _UNICODE
			adapter.name = dev_string;
#else
			adapter.name = adapter.display_device.DeviceString;
#endif

			it_disp = _display_info.insert(it_disp, adapter);
		}

		GleamArray<OutputInfo>::Iterator it_out = it_disp->output_info.linearSearch(dev_name.getBuffer(), [](const OutputInfo& lhs, const char* rhs) -> bool
		{
			return lhs.name == rhs;
		});

		if (it_out == it_disp->output_info.end()) {
			OutputInfo display;

#ifdef _UNICODE
			display.name = dev_name;
#else
			display.name = disp_device.DeviceName;
#endif

			it_out = it_disp->output_info.insert(it_out, display);
		}

		for (unsigned int j = 0; EnumDisplaySettings(disp_device.DeviceName, j, &mode); ++j) {
			if (mode.dmBitsPerPel != 32) {
				continue;
			}

			it_out->display_mode_list.push(mode);
		}

		// Remove duplicate entries. We don't care about the scaling or scanline order or other funky stuff.
		for (unsigned int j = 1; j < it_out->display_mode_list.size();) {
			const DEVMODE& curr = it_out->display_mode_list[j];
			const DEVMODE& prev = it_out->display_mode_list[j - 1];

			if (curr.dmPelsWidth == prev.dmPelsWidth && curr.dmPelsHeight == prev.dmPelsHeight &&
				curr.dmDisplayFrequency == prev.dmDisplayFrequency) {

				it_out->display_mode_list.erase(j);

			} else {
				++j;
			}
		}
	}

	_display_info.trim();

	// Convert Windows data structures into our structure
	AdapterList out(_display_info.size());

	for (unsigned int i = 0; i < _display_info.size(); ++i) {
		const AdapterInfo& adpt_info = _display_info[i];
		Adapter adpt;

		adpt.displays.reserve(adpt_info.output_info.size());
		adpt.memory = 0;
		adpt.id = i;

		strncpy(adpt.adapter_name, adpt_info.name.getBuffer(), 128);

		for (unsigned int j = 0; j < adpt_info.output_info.size(); ++j) {
			const OutputInfo& out_info = adpt_info.output_info[j];
			Display display;
			display.display_modes.reserve(out_info.display_mode_list.size());
			display.id = j;

			for (unsigned int k = 0; k < out_info.display_mode_list.size(); ++k) {
				const DEVMODE& mode_desc = out_info.display_mode_list[k];

				DisplayMode disp_mode = {
					mode_desc.dmDisplayFrequency,
					mode_desc.dmPelsWidth,
					mode_desc.dmPelsHeight,
					k,
					mode_desc.dmPosition.x,
					mode_desc.dmPosition.y
				};

				display.display_modes.push(disp_mode);
			}

			adpt.displays.push(display);
		}

		out.push(adpt);
	}

	return out;
}

bool RenderDeviceGL::init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync)
{
	GAFF_ASSERT(
		_display_info.size() > adapter_id &&
		_display_info[adapter_id].output_info.size() > display_id &&
		_display_info[adapter_id].output_info[display_id].display_mode_list.size() > display_mode_id
	);

	const Window& wnd = (const Window&)window;
	HWND hwnd = wnd.getHWnd();

	// Move the window to the intended adapter/display combination, in hopes of forcing this window to be associated with the correct adapter
	DEVMODE& mode = _display_info[adapter_id].output_info[display_id].display_mode_list[display_mode_id];
	MoveWindow(hwnd, mode.dmPosition.x, mode.dmPosition.y, mode.dmPelsWidth, mode.dmPelsHeight, TRUE);

	if (wnd.isFullScreen()) {
		if (vsync) {
			mode.dmFields |= DM_DISPLAYFREQUENCY;
		} else {
			mode.dmFields &= ~DM_DISPLAYFREQUENCY;
		}

		ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
	}

	HDC hdc = GetDC(hwnd);

	if (!hwnd || !hdc) {
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.dwLayerMask = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 0;
	pfd.cAccumBits = 0;
	pfd.cStencilBits = 0;

	int pixelformat = ChoosePixelFormat(hdc, &pfd);

	if (!pixelformat || !SetPixelFormat(hdc, pixelformat, &pfd)) {
		ReleaseDC(hwnd, hdc);
		return false;
	}

	GleamArray<Device>::Iterator it = _devices.linearSearch(adapter_id, [](const Device& lhs, unsigned int rhs) -> bool
	{
		return lhs.adapter_id == rhs;
	});

	HGLRC context = wglCreateContext(hdc);

	if (!context) {
		ReleaseDC(hwnd, hdc);
		return false;
	}

	// We didn't find the device, so just make it and add it to the list
	if (it == _devices.end()) {
		Device device;

		Viewport viewport = { 0, 0, (int)wnd.getWidth(), (int)wnd.getHeight() };

		device.contexts.push(context);
		device.viewports.push(viewport);
		device.windows.push(hwnd);
		device.outputs.push(hdc);
		device.vsync.push(vsync);
		device.adapter_id = adapter_id;

		RenderTargetGL* rt = GleamAllocateT(RenderTargetGL);
		rt->setViewport(viewport.width, viewport.height);
		device.rts.push(IRenderTargetPtr(rt));

		_devices.push(device);

	// We found the device already made, so add our new outputs to the list
	} else {
		// Well that sucks. We went through all this work and we can't even share our display lists? :(
		if (wglShareLists(it->contexts[0], context) == FALSE) {
			ReleaseDC(hwnd, hdc);
			wglDeleteContext(context);
			return false;
		}

		Viewport viewport = { 0, 0, (int)wnd.getWidth(), (int)wnd.getHeight() };
		it->contexts.push(context);
		it->viewports.push(viewport);
		it->windows.push(hwnd);
		it->outputs.push(hdc);
		it->vsync.push(vsync);

		RenderTargetGL* rt = GleamAllocateT(RenderTargetGL);
		rt->setViewport(viewport.width, viewport.height);
		it->rts.push(IRenderTargetPtr(rt));
	}

	if (!_glew_already_initialized) {
		if (!wglMakeCurrent(hdc, context)) {
			return false;
		}
		
		if (glewInit() != GLEW_OK) {
			return false;
		}

		if (WGLEW_EXT_swap_control) {
			wglSwapIntervalEXT(vsync);
		}

		_glew_already_initialized = true;
		setCurrentDevice(0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	}

	return true;
}

void RenderDeviceGL::destroy(void)
{
	wglMakeCurrent(0, 0);

	for (auto it = _devices.begin(); it != _devices.end(); ++it) {
		for (unsigned int i = 0; i < it->outputs.size(); ++i) {
			ReleaseDC(it->windows[i], it->outputs[i]);
			wglDeleteContext(it->contexts[i]);
		}
	}

	_devices.clear();
}

bool RenderDeviceGL::isVsync(unsigned int device, unsigned int output) const
{
	GAFF_ASSERT(_devices.size() > device && _devices[device].vsync.size() > output);
	return _devices[device].vsync[output];
}

void RenderDeviceGL::setVsync(bool vsync, unsigned int device, unsigned int output)
{
	GAFF_ASSERT(_devices.size() > device && _devices[device].vsync.size() > output);
	_devices[device].vsync.setBit(output, vsync);
}

void RenderDeviceGL::beginFrame(void)
{
	GAFF_ASSERT(_devices.size() > _curr_device && _devices[_curr_device].outputs.size() > _curr_output);
	glViewport(_active_viewport->x, _active_viewport->y, _active_viewport->width, _active_viewport->height);
	resetRenderState();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
}

void RenderDeviceGL::endFrame(void)
{
	GAFF_ASSERT(_devices.size() > _curr_device && _devices[_curr_device].outputs.size() > _curr_output);
	SwapBuffers(_active_output);
}

bool RenderDeviceGL::resize(const IWindow& window)
{
	const Window& wnd = reinterpret_cast<const Window&>(window);

	for (auto it = _devices.begin(); it != _devices.end(); ++it) {
		size_t index = it->windows.linearSearch(0, it->windows.size(), wnd.getHWnd());

		if (index != SIZE_T_FAIL) {
			it->viewports[index].width = wnd.getWidth();
			it->viewports[index].height = wnd.getHeight();
			reinterpret_cast<RenderTargetGL*>(it->rts[index].get())->setViewport(wnd.getWidth(), wnd.getHeight());

			if (wnd.getHWnd() == _devices[_curr_device].windows[_curr_output]) {
				glViewport(0, 0, wnd.getWidth(), wnd.getHeight());
			}

			// Maybe detect and handle fullscreen changes here?
			break;
		}
	}

	return true;
}

bool RenderDeviceGL::handleFocusGained(const IWindow&)
{
	return true;
}

unsigned int RenderDeviceGL::getViewportWidth(unsigned int device, unsigned int output) const
{
	GAFF_ASSERT(_devices.size() > device && _devices[device].viewports.size() > output);
	return static_cast<unsigned int>(_devices[device].viewports[output].width);
}

unsigned int RenderDeviceGL::getViewportHeight(unsigned int device, unsigned int output) const
{
	GAFF_ASSERT(_devices.size() > device && _devices[device].viewports.size() > output);
	return static_cast<unsigned int>(_devices[device].viewports[output].height);
}

unsigned int RenderDeviceGL::getActiveViewportWidth(void)
{
	GAFF_ASSERT(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);
	return getViewportWidth(_curr_device, _curr_output);
}

unsigned int RenderDeviceGL::getActiveViewportHeight(void)
{
	GAFF_ASSERT(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);
	return getViewportHeight(_curr_device, _curr_output);
}

unsigned int RenderDeviceGL::getNumOutputs(unsigned int device) const
{
	GAFF_ASSERT(_devices.size() > device);
	return static_cast<unsigned int>(_devices[device].outputs.size());
}

unsigned int RenderDeviceGL::getNumDevices(void) const
{
	return static_cast<unsigned int>(_devices.size());
}

IRenderTargetPtr RenderDeviceGL::getOutputRenderTarget(unsigned int device, unsigned int output)
{
	GAFF_ASSERT(_devices.size() > device && _devices[device].rts.size() > output);
	return _devices[device].rts[output];
}

IRenderTargetPtr RenderDeviceGL::getActiveOutputRenderTarget(void)
{
	GAFF_ASSERT(_devices.size() > _curr_device && _devices[_curr_device].rts.size() > _curr_output);
	return getOutputRenderTarget(_curr_device, _curr_output);
}

bool RenderDeviceGL::setCurrentOutput(unsigned int output)
{
	GAFF_ASSERT(_devices[_curr_device].outputs.size() > output);

	if (_curr_output == output) {
		return true;
	}

	unsigned int thread_id = Gaff::Thread::GetCurrentThreadID();

	if (wglMakeCurrent(_devices[_curr_device].outputs[output], _thread_contexts[thread_id][_curr_device][output]) == FALSE) {
		return false;
	}

	if (WGLEW_EXT_swap_control) {
		if (wglSwapIntervalEXT(_devices[_curr_device].vsync[output]) == FALSE) {
			return false;
		}
	}

	_active_viewport = &_devices[_curr_device].viewports[output];
	_active_output = _devices[_curr_device].outputs[output];
	_curr_output = output;

	return true;
}

unsigned int RenderDeviceGL::getCurrentOutput(void) const
{
	return _curr_output;
}

// Fix this for proper thread local storage
bool RenderDeviceGL::setCurrentDevice(unsigned int device)
{
	GAFF_ASSERT(_devices.size() > device && _devices[device].vsync.size() > 0);
	unsigned int prev_device = _curr_device;
	unsigned int prev_output = _curr_output;
	_curr_device = device;
	_curr_output = UINT_FAIL;

	if (_curr_device != prev_device && !setCurrentOutput(0)) {
		_curr_device = prev_device;
		_curr_output = prev_output;

		setCurrentOutput(_curr_output); // attempt to set back to the old device
		return false;
	}

	return true;
}

unsigned int RenderDeviceGL::getCurrentDevice(void) const
{
	return _curr_device;
}

unsigned int RenderDeviceGL::getDeviceForAdapter(unsigned int adapter_id) const
{
	for (unsigned int i = 0; i < _devices.size(); ++i) {
		if (_devices[i].adapter_id == adapter_id) {
			return i;
		}
	}

	return UINT_FAIL;
}

unsigned int RenderDeviceGL::getDeviceForMonitor(unsigned int monitor) const
{
	unsigned int num_outputs = 0;

	for (unsigned int i = 0; i < _devices.size(); ++i) {
		num_outputs += getNumOutputs(i);

		if (num_outputs >= monitor) {
			return i;
		}
	}

	return UINT_FAIL;
}


NS_END

#endif
