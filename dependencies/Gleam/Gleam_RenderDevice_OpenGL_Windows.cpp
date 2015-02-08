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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_RenderDevice_OpenGL_Windows.h"
#include "Gleam_RenderTarget_OpenGL.h"
#include "Gleam_CommandList_OpenGL.h"
#include "Gleam_Window_Windows.h"

#include "Gleam_ShaderResourceView_OpenGL.h"
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_RenderState_OpenGL.h"
#include "Gleam_Program_OpenGL.h"
#include "Gleam_Layout_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include "Gleam_Mesh_OpenGL.h"
#include "Gleam_IMesh.h"

#include <Gaff_IncludeAssert.h>
#include <Gaff_Thread.h>
#include <GL/glew.h>
#include <GL/wglew.h>

NS_GLEAM

RenderDeviceGL::RenderDeviceGL(void):
	_active_viewport(nullptr), _active_output(nullptr),
	_curr_output(UINT_FAIL), _curr_device(UINT_FAIL), _creating_thread_id(0),
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

bool RenderDeviceGL::initThreadData(unsigned int* thread_ids, size_t num_ids)
{
	for (unsigned int i = 0; i < num_ids; ++i) {
		GleamArray<HGLRC>& thread_data = _thread_contexts[thread_ids[i]];

		thread_data.reserve(_devices.size());

		for (unsigned int j = 0; j < _devices.size(); ++j) {
			HGLRC context = wglCreateContext(_devices[j].outputs[0]);

			if (!context) {
				return false;
			}

			if (wglShareLists(_devices[j].contexts[0], context) == FALSE) {
				wglDeleteContext(context);
				return false;
			}

			thread_data.push(context);
		}
	}

	return true;
}

IRenderDevice::AdapterList RenderDeviceGL::getDisplayModes(int)
{
	DISPLAY_DEVICE disp_device;
	DEVMODE mode;

	disp_device.cb = sizeof(DISPLAY_DEVICE);

	for (unsigned int i = 0; EnumDisplayDevices(nullptr, i, &disp_device, EDD_GET_DEVICE_INTERFACE_NAME); ++i) {
		if (!(disp_device.StateFlags & DISPLAY_DEVICE_ACTIVE)) {
			continue;
		}

		// Find an entry that already exists. We might be a different display on the same device.
		GleamArray<AdapterInfo>::Iterator it_disp = _display_info.linearSearch(disp_device.DeviceString, [](const AdapterInfo& lhs, const GChar* rhs) -> bool
		{
			return lhs.name == rhs;
		});

		if (it_disp == _display_info.end()) {
			AdapterInfo adapter;
			adapter.display_device = disp_device;
			adapter.name = adapter.display_device.DeviceString;

			it_disp = _display_info.insert(adapter, it_disp);
		}

		GleamArray<OutputInfo>::Iterator it_out = it_disp->output_info.linearSearch(disp_device.DeviceName, [](const OutputInfo& lhs, const GChar* rhs) -> bool
		{
			return lhs.name == rhs;
		});

		if (it_out == it_disp->output_info.end()) {
			OutputInfo display;
			display.name = disp_device.DeviceName;

			it_out = it_disp->output_info.insert(display, it_out);
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

#ifdef _UNICODE
		wcstombs(adpt.adapter_name, adpt_info.name.getBuffer(), 128);
#else
		strncpy(adpt.adapter_name, adpt_info.name.getBuffer(), 128);
#endif

		for (unsigned int j = 0; j < adpt_info.output_info.size(); ++j) {
			const OutputInfo& out_info = adpt_info.output_info[j];
			Display display;
			display.display_modes.reserve(out_info.display_mode_list.size());
			display.id = j;

			for (unsigned int k = 0; k < out_info.display_mode_list.size(); ++k) {
				const DEVMODE& mode_desc = out_info.display_mode_list[k];

				DisplayMode mode = {
					mode_desc.dmDisplayFrequency,
					mode_desc.dmPelsWidth,
					mode_desc.dmPelsHeight,
					k,
					mode_desc.dmPosition.x,
					mode_desc.dmPosition.y
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

#ifdef _DEBUG
	if (_creating_thread_id) {
		assert(_creating_thread_id == Gaff::Thread::getCurrentThreadID());
	}
#endif

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

	PIXELFORMATDESCRIPTOR pfd;
	int pixelformat;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.dwLayerMask = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_COLORINDEX;
	pfd.cColorBits = 8;
	pfd.cDepthBits = 0;
	pfd.cAccumBits = 0;
	pfd.cStencilBits = 0;

	pixelformat = ChoosePixelFormat(hdc, &pfd);

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

		if (!_creating_thread_id) {
			_creating_thread_id = Gaff::Thread::getCurrentThreadID();
		}

		_glew_already_initialized = true;
		setCurrentDevice(0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	} else {
		if (!_creating_thread_id) {
			_creating_thread_id = Gaff::Thread::getCurrentThreadID();
		}
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
	assert(_devices.size() > _curr_device && _devices[_curr_device].outputs.size() > _curr_output);
	glViewport(_active_viewport->x, _active_viewport->y, _active_viewport->width, _active_viewport->height);
	resetRenderState();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
}

void RenderDeviceGL::endFrame(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].outputs.size() > _curr_output);
	SwapBuffers(_active_output);
}

bool RenderDeviceGL::resize(const IWindow& window)
{
	const Window& wnd = (const Window&)window;

	for (auto it = _devices.begin(); it != _devices.end(); ++it) {
		size_t index = it->windows.linearSearch(0, it->windows.size(), wnd.getHWnd());

		if (index != SIZE_T_FAIL) {
			it->viewports[index].width = wnd.getWidth();
			it->viewports[index].height = wnd.getHeight();
			((RenderTargetGL*)it->rts[index].get())->setViewport(wnd.getWidth(), wnd.getHeight());

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

bool RenderDeviceGL::isDeferred(void) const
{
	return false;
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
	return static_cast<unsigned int>(_devices[device].outputs.size());
}

unsigned int RenderDeviceGL::getNumDevices(void) const
{
	return static_cast<unsigned int>(_devices.size());
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
	assert(_devices[_curr_device].outputs.size() > output);

	if (_curr_output == output) {
		return true;
	}

	if (wglMakeCurrent(_devices[_curr_device].outputs[output], _devices[_curr_device].contexts[output]) == FALSE) {
		return false;
	}

	if (WGLEW_EXT_swap_control) {
		if (wglSwapIntervalEXT(_devices[_curr_device].vsync[0]) == FALSE) {
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

bool RenderDeviceGL::setCurrentDevice(unsigned int device)
{
	assert(_devices.size() > device && _devices[device].vsync.size() > 0);
	unsigned int prev_device = _curr_device;
	_curr_device = device;

	unsigned int thread_id = Gaff::Thread::getCurrentThreadID();

	if (thread_id != _creating_thread_id) {
		_curr_device = prev_device;

		GleamArray<HGLRC>& contexts = _thread_contexts[thread_id];

		if (wglMakeCurrent(_devices[_curr_device].outputs[0], contexts[device]) == FALSE) {
			return false;
		}


	} else if (_curr_device != prev_device && !setCurrentOutput(0)) {
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

unsigned int RenderDeviceGL::getDeviceForAdapter(unsigned int adapter_id) const
{
	for (unsigned int i = 0; i < _devices.size(); ++i) {
		if (_devices[i].adapter_id == adapter_id) {
			return i;
		}
	}

	return UINT_FAIL;
}

IRenderDevice* RenderDeviceGL::createDeferredRenderDevice(void)
{
	return nullptr;
}

void RenderDeviceGL::executeCommandList(ICommandList* command_list)
{
	assert(!command_list->isD3D());
	resetRenderState();
	((CommandListGL*)command_list)->execute();
}

bool RenderDeviceGL::finishCommandList(ICommandList*)
{
	assert(0 && "Calling a deferred render device function on an immediate render device");
	return false;
}

void RenderDeviceGL::setRenderState(const RenderStateGL* render_state)
{
	glPolygonMode(GL_FRONT_AND_BACK, (render_state->isWireframe()) ? GL_LINE : GL_FILL);

	RenderStateGL::Disable_Enable_Funcs[render_state->isDepthTestEnabled()](GL_DEPTH_TEST);
	RenderStateGL::Disable_Enable_Funcs[render_state->isStencilTestEnabled()](GL_STENCIL_TEST);
	glDepthFunc(RenderStateGL::Compare_Funcs[render_state->getDepthFunc() - 1]);

	const IRenderState::StencilData& front_face = render_state->getFrontFaceStencilData();
	const IRenderState::StencilData& back_face = render_state->getBackFaceStencilData();

	glStencilOpSeparate(GL_FRONT, RenderStateGL::Stencil_Ops[front_face.stencil_depth_fail - 1],
						RenderStateGL::Stencil_Ops[front_face.stencil_pass_depth_fail - 1],
						RenderStateGL::Stencil_Ops[front_face.stencil_depth_pass - 1]);
	glStencilOpSeparate(GL_BACK, RenderStateGL::Stencil_Ops[back_face.stencil_depth_fail - 1],
						RenderStateGL::Stencil_Ops[back_face.stencil_pass_depth_fail - 1],
						RenderStateGL::Stencil_Ops[back_face.stencil_depth_pass - 1]);
	glStencilFuncSeparate(GL_FRONT, RenderStateGL::Compare_Funcs[front_face.comp_func - 1], render_state->getStencilRef(), render_state->getStencilReadMask());
	glStencilFuncSeparate(GL_BACK, RenderStateGL::Compare_Funcs[back_face.comp_func - 1], render_state->getStencilRef(), render_state->getStencilWriteMask());
	glStencilMask(render_state->getStencilWriteMask());

	IRenderState::CULL_MODE cull_face_mode = render_state->getCullFaceMode();

	RenderStateGL::Disable_Enable_Funcs[cull_face_mode != IRenderState::CULL_NONE](GL_CULL_FACE);
	glCullFace(GL_FRONT - 2 + cull_face_mode);
	glFrontFace((render_state->isFrontFaceCounterClockwise()) ? GL_CCW : GL_CW);

	const IRenderState::BlendData* blend_data = render_state->getBlendData();

	for (unsigned int i = 0; i < 8; ++i) {
		if (blend_data[i].enable_alpha_blending) {
			glEnablei(GL_BLEND, i);

			glBlendFuncSeparatei(i, RenderStateGL::Blend_Factors[blend_data[i].blend_src_color - 1], RenderStateGL::Blend_Factors[blend_data[i].blend_dst_color - 1],
								RenderStateGL::Blend_Factors[blend_data[i].blend_src_alpha - 1], RenderStateGL::Blend_Factors[blend_data[i].blend_dst_alpha - 1]);

			glBlendEquationSeparatei(i, RenderStateGL::Blend_Ops[blend_data[i].blend_op_color - 1], RenderStateGL::Blend_Ops[blend_data[i].blend_op_alpha - 1]);

		} else {
			glDisablei(GL_BLEND, i);
		}
	}
}

void RenderDeviceGL::setLayout(LayoutGL* layout, const IMesh* mesh)
{
	const GleamArray< GleamArray<LayoutGL::LayoutData> >& layout_descs = layout->GetLayoutDescriptors();
	const LayoutGL::LayoutData* layout_data = nullptr;
	const BufferGL* buffer = nullptr;
	unsigned int stride = 0;
	unsigned int count = 0;

	for (unsigned int i = 0; i < layout_descs.size(); ++i) {
		const GleamArray<LayoutGL::LayoutData>& ld = layout_descs[i];
		buffer = (const BufferGL*)mesh->getBuffer(i);
		stride = buffer->getStride();

		glBindBuffer(GL_ARRAY_BUFFER, buffer->getBuffer());

		for (unsigned int j = 0; j < ld.size(); ++j) {
			layout_data = &ld[j];

			glEnableVertexAttribArray(count);

#if defined(_WIN64) || defined(__LP64__)
			glVertexAttribPointer(count, layout_data->size, layout_data->type, layout_data->normalized,
								stride, (void*)(unsigned long long)layout_data->aligned_byte_offset);
#else
			glVertexAttribPointer(count, layout_data->size, layout_data->type, layout_data->normalized,
								stride, (void*)layout_data->aligned_byte_offset);
#endif

			++count;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderDeviceGL::unsetLayout(LayoutGL* layout)
{
	const GleamArray< GleamArray<LayoutGL::LayoutData> >& layout_descs = layout->GetLayoutDescriptors();
	unsigned int count = 0;

	for (unsigned int i = 0; i < layout_descs.size(); ++i) {
		for (unsigned int j = 0; j < layout_descs[i].size(); ++j) {
			glDisableVertexAttribArray(count);
			++count;
		}
	}
}

void RenderDeviceGL::bindShader(ProgramGL* shader, ProgramBuffersGL* program_buffers)
{
	glBindProgramPipeline(shader->getProgram());

	if (program_buffers) {
		unsigned int texture_count = 0;
		unsigned int count = 0;

		for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
			const GleamArray<IShaderResourceView*>& resource_views = program_buffers->getResourceViews((Gleam::IShader::SHADER_TYPE)i);
			const GleamArray<ISamplerState*>& sampler_states = program_buffers->getSamplerStates((Gleam::IShader::SHADER_TYPE)i);
			const GleamArray<IBuffer*>& const_bufs = program_buffers->getConstantBuffers((Gleam::IShader::SHADER_TYPE)i);

			assert(sampler_states.size() <= resource_views.size());
			unsigned int sampler_count = 0;

			for (unsigned int j = 0; j < const_bufs.size(); ++j) {
				glBindBufferBase(GL_UNIFORM_BUFFER, count, ((const BufferGL*)const_bufs[j])->getBuffer());
				++count;
			}

			for (unsigned int j = 0; j < resource_views.size(); ++j) {
				ShaderResourceViewGL* rv = (ShaderResourceViewGL*)resource_views[j];

				if (resource_views[j]->getViewType() == IShaderResourceView::VIEW_TEXTURE) {
					// should probably assert that texture_count isn't higher than the supported number of textures

					SamplerStateGL* st = (SamplerStateGL*)sampler_states[sampler_count];

					glActiveTexture(GL_TEXTURE0 + texture_count);
					glBindTexture(rv->getTarget(), rv->getResourceView());
					glBindSampler(texture_count, st->getSamplerState());

					++texture_count;
					++sampler_count;

				} else {
					assert(0 && "How is your ShaderResourceView not a texture? That's the only type we have implemented ...");
				}
			}
		}
	}
}

void RenderDeviceGL::unbindShader(void)
{
	glBindProgramPipeline(0);
}

void RenderDeviceGL::renderMeshNonIndexed(unsigned int topology, unsigned int vert_count, unsigned int start_location)
{
	glDrawArrays(topology, start_location, vert_count);
}

void RenderDeviceGL::renderMeshInstanced(MeshGL* mesh, unsigned int count)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((BufferGL*)mesh->getIndiceBuffer())->getBuffer());
	glDrawElementsInstanced(mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0, count);
}

void RenderDeviceGL::renderMesh(MeshGL* mesh)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((BufferGL*)mesh->getIndiceBuffer())->getBuffer());
	glDrawElements(mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
}

NS_END

#endif