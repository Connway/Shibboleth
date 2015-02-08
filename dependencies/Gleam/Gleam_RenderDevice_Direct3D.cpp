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

#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_DeferredRenderDevice_Direct3D.h"
#include "Gleam_RenderTarget_Direct3D.h"
#include "Gleam_CommandList_Direct3D.h"
#include "Gleam_Window_Windows.h"
#include "Gleam_Global.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

RenderDeviceD3D::RenderDeviceD3D(void):
	_video_memory(0), _curr_output(UINT_FAIL), _curr_device(UINT_FAIL)
{
	_clear_color[0] = 0.0f;
	_clear_color[1] = 0.0f;
	_clear_color[2] = 0.0f;
	_clear_color[3] = 1.0f;
}

RenderDeviceD3D::~RenderDeviceD3D(void)
{
	destroy();
}

IRenderDevice::AdapterList RenderDeviceD3D::getDisplayModes(int color_format)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapter_output;
	DXGI_ADAPTER_DESC adapter_desc;

	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	if (FAILED(result)) {
		return AdapterList();
	}

	for (unsigned int i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		AdapterInfo info;
		info.adapter.set(adapter);

		result = adapter->GetDesc(&adapter_desc);

		if (FAILED(result)) {
			continue;
		}

		info.memory = (UINT)(adapter_desc.DedicatedVideoMemory / 1024) / 1024;
		wcsncpy_s(info.adapter_name, 128, adapter_desc.Description, 128);

		for (unsigned int j = 0; adapter->EnumOutputs(j, &adapter_output) != DXGI_ERROR_NOT_FOUND; ++j) {
			OutputInfo out_info;
			out_info.output.set(adapter_output);

			unsigned int num_modes;

			result = adapter_output->GetDisplayModeList((DXGI_FORMAT)color_format, 0UL, &num_modes, nullptr);

			if (FAILED(result)) {
				continue;
			}

			out_info.display_mode_list.resize(num_modes);
			adapter_output->GetDisplayModeList((DXGI_FORMAT)color_format, 0UL, &num_modes, out_info.display_mode_list.getArray());

			// Remove duplicate entries. We don't care about the scaling or scanline order
			for (unsigned int k = 1; k < out_info.display_mode_list.size();) {
				const DXGI_MODE_DESC& curr = out_info.display_mode_list[k];
				const DXGI_MODE_DESC& prev = out_info.display_mode_list[k - 1];

				if (curr.Width == prev.Width && curr.Height == prev.Height &&
					curr.RefreshRate.Numerator == prev.RefreshRate.Numerator &&
					curr.RefreshRate.Denominator == prev.RefreshRate.Denominator) {

					out_info.display_mode_list.erase(k);

				} else {
					++k;
				}
			}

			info.output_info.push(out_info);
		}

		_display_info.push(info);
	}

	factory->Release();

	_display_info.trim();

	// convert DirectX data structures into our structure
	AdapterList out(_display_info.size());

	for (unsigned int i = 0; i < _display_info.size(); ++i) {
		const AdapterInfo& adpt_info = _display_info[i];
		Adapter adpt;

		adpt.displays.reserve(adpt_info.output_info.size());
		wcstombs(adpt.adapter_name, adapter_desc.Description, 128);
		adpt.memory = adpt_info.memory;
		adpt.id = i;

		for (unsigned int j = 0; j < adpt_info.output_info.size(); ++j) {
			const OutputInfo& out_info = adpt_info.output_info[j];
			Display display;
			display.display_modes.reserve(out_info.display_mode_list.size());
			display.id = j;

			for (unsigned int k = 0; k < out_info.display_mode_list.size(); ++k) {
				const DXGI_MODE_DESC& mode_desc = out_info.display_mode_list[k];

				DisplayMode mode = {
					mode_desc.RefreshRate.Numerator / mode_desc.RefreshRate.Denominator,
					mode_desc.Width,
					mode_desc.Height,
					k,
					0, 0
				};

				display.display_modes.push(mode);
			}

			adpt.displays.push(display);
		}

		out.push(adpt);
	}

	return out;
}

bool RenderDeviceD3D::initThreadData(unsigned int*, size_t)
{
	return true;
}

bool RenderDeviceD3D::init(const IWindow& window, unsigned int adapter_id, unsigned int display_id, unsigned int display_mode_id, bool vsync)
{
	assert(	_display_info.size() > adapter_id &&
			_display_info[adapter_id].output_info.size() > display_id &&
			_display_info[adapter_id].output_info[display_id].display_mode_list.size() > display_id
	);

	Window& wnd = (Window&)window;

	AdapterInfo& adapter =_display_info[adapter_id];
	const DXGI_MODE_DESC& mode = adapter.output_info[display_id].display_mode_list[display_mode_id];

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc = mode;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = wnd.getHWnd();
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (!vsync) {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
	}

	GleamArray<Device>::Iterator it = _devices.linearSearch(adapter_id, [](const Device& lhs, unsigned int rhs) -> bool
	{
		return lhs.adapter_id == rhs;
	});

	// We didn't find the device, so just make it and add it to the list
	if (it == _devices.end())
	{
		ID3D11DeviceContext* device_context = nullptr;
		IDXGISwapChain* swap_chain = nullptr;
		ID3D11Device* device = nullptr;

// Apparently trying to make the device as debug/debuggable causes it to fail. :/
//#ifdef _DEBUG
//		unsigned int flags = /*D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE*/ 0;
//#else
		unsigned int flags = 0;
//#endif

		D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
		HRESULT result = D3D11CreateDeviceAndSwapChain(adapter.adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, 0, flags, &feature_level, 1, D3D11_SDK_VERSION,
														&swap_chain_desc, &swap_chain, &device, 0, &device_context);

		RETURNIFFAILED(result)

		ID3D11Texture2D* back_buffer_ptr = nullptr;
		result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);

		if (FAILED(result)) {
			swap_chain->Release();
			device_context->Release();
			device->Release();
			return false;
		}

		ID3D11RenderTargetView* render_target_view = nullptr;
		result = device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);

		back_buffer_ptr->Release();

		if (FAILED(result)) {
			swap_chain->Release();
			device_context->Release();
			device->Release();
			return false;
		}

		if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
			result = swap_chain->SetFullscreenState(TRUE, _display_info[adapter_id].output_info[display_id].output.get());
		} else {
			result = swap_chain->SetFullscreenState(FALSE, nullptr);
		}

		Gaff::COMRefPtr<ID3D11RenderTargetView> rtv;
		Gaff::COMRefPtr<IDXGISwapChain> sc;
		rtv.set(render_target_view);
		sc.set(swap_chain);

		RETURNIFFAILED(result)

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)wnd.getWidth();
		viewport.Height = (float)wnd.getHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		Device dvc;
		dvc.render_targets.push(rtv);
		dvc.swap_chains.push(sc);
		dvc.viewports.push(viewport);
		dvc.context.set(device_context);
		dvc.device.set(device);
		dvc.vsync.push(vsync);
		dvc.adapter_id = adapter_id;

		RenderTargetD3D* rt = GleamAllocateT(RenderTargetD3D);
		rt->setRTV(rtv.get(), viewport);

		dvc.gleam_rts.push(IRenderTargetPtr(rt));

		_devices.push(dvc);

	// We found the device already made, so make a new swap chain and render target and add them to the list
	} else {
		IDXGISwapChain* swap_chain = nullptr;
		IDXGIFactory* factory = nullptr;

		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		RETURNIFFAILED(result)

		result = factory->CreateSwapChain(it->device.get(), &swap_chain_desc, &swap_chain);

		factory->Release();
		RETURNIFFAILED(result)

		ID3D11Texture2D* back_buffer_ptr = nullptr;
		result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);

		if (FAILED(result)) {
			swap_chain->Release();
			return false;
		}

		ID3D11RenderTargetView* render_target_view = nullptr;
		result = it->device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);

		back_buffer_ptr->Release();

		if (FAILED(result)) {
			swap_chain->Release();
			return false;
		}

		if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
			result = swap_chain->SetFullscreenState(TRUE, _display_info[adapter_id].output_info[display_id].output.get());
		} else {
			result = swap_chain->SetFullscreenState(FALSE, nullptr);
		}

		Gaff::COMRefPtr<ID3D11RenderTargetView> rtv;
		Gaff::COMRefPtr<IDXGISwapChain> sc;
		rtv.set(render_target_view);
		sc.set(swap_chain);

		RETURNIFFAILED(result)

		it->render_targets.push(rtv);
		it->swap_chains.push(sc);
		it->vsync.push(vsync);

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)wnd.getWidth();
		viewport.Height = (float)wnd.getHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		it->viewports.push(viewport);

		RenderTargetD3D* rt = GleamAllocateT(RenderTargetD3D);
		rt->setRTV(rtv.get(), viewport);

		it->gleam_rts.push(IRenderTargetPtr(rt));
	}

	if (!_active_device) {
		setCurrentDevice(0);
	}

	return true;
}

void RenderDeviceD3D::destroy(void)
{
	_display_info.clear();
	_devices.clear();
}

bool RenderDeviceD3D::isVsync(unsigned int device, unsigned int output) const
{
	assert(_devices.size() > device && _devices[device].vsync.size() > output);
	return _devices[device].vsync[output];
}

void RenderDeviceD3D::setVsync(bool vsync, unsigned int device, unsigned int output)
{
	assert(_devices.size() > device && _devices[device].vsync.size() > output);
	_devices[device].vsync.setBit(output, vsync);
}

void RenderDeviceD3D::setClearColor(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;
}

const float* RenderDeviceD3D::getClearColor(void) const
{
	return _clear_color;
}

void RenderDeviceD3D::beginFrame(void)
{
	resetRenderState();
	_active_context->RSSetViewports(1, &_active_viewport);
	_active_context->ClearRenderTargetView(_active_render_target.get(), _clear_color);
}

void RenderDeviceD3D::endFrame(void)
{
	_active_swap_chain->Present(_active_vsync, 0);
}

bool RenderDeviceD3D::resize(const IWindow& window)
{
	Window& wnd = (Window&)window;

	for (unsigned int i = 0; i < _devices.size(); ++i) {
		Device& device = _devices[i];

		for (unsigned int j = 0; j < device.swap_chains.size(); ++j) {
			Gaff::COMRefPtr<ID3D11RenderTargetView>& rtv = device.render_targets[j];
			Gaff::COMRefPtr<IDXGISwapChain>& sc = device.swap_chains[j];
			IRenderTargetPtr rt = device.gleam_rts[j];
			D3D11_VIEWPORT& viewport = device.viewports[j];
			DXGI_SWAP_CHAIN_DESC sc_desc;

			if (SUCCEEDED(sc->GetDesc(&sc_desc))) {
				if (sc_desc.OutputWindow == wnd.getHWnd()) {
					bool resizing_active_output = false;

					if (rtv == _active_render_target) {
						resizing_active_output = true;
						_active_render_target = nullptr;
						_active_swap_chain = nullptr;
					}

					rtv = nullptr;
					rt->destroy();
					HRESULT result = sc->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
					RETURNIFFAILED(result)

					ID3D11Texture2D* back_buffer_ptr;
					result = sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
					RETURNIFFAILED(result)

					ID3D11RenderTargetView* render_target_view = nullptr;
					result = device.device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);
					back_buffer_ptr->Release();
					RETURNIFFAILED(result)

					viewport.Width = (float)wnd.getWidth();
					viewport.Height = (float)wnd.getHeight();
					rtv.set(render_target_view);
					((RenderTargetD3D*)rt.get())->setRTV(render_target_view, viewport);

					if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
						result = sc->SetFullscreenState(TRUE, _display_info[i].output_info[j].output.get());
					} else {
						result = sc->SetFullscreenState(FALSE, nullptr);
					}

					if (resizing_active_output) {
						setCurrentOutput(getCurrentOutput());
					}

					return SUCCEEDED(result);
				}
			}
		}
	}

	return false;
}

bool RenderDeviceD3D::handleFocusGained(const IWindow& window)
{
	const Window& wnd = (const Window&)window;

	for (unsigned int i = 0; i < _devices.size(); ++i) {
		Device& device = _devices[i];

		for (unsigned int j = 0; j < device.swap_chains.size(); ++j) {
			Gaff::COMRefPtr<IDXGISwapChain>& sc = device.swap_chains[j];
			DXGI_SWAP_CHAIN_DESC sc_desc;

			if (SUCCEEDED(sc->GetDesc(&sc_desc))) {
				if (sc_desc.OutputWindow == wnd.getHWnd()) {
					if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
						return SUCCEEDED(sc->SetFullscreenState(TRUE, _display_info[i].output_info[j].output.get()));
					} else {
						return SUCCEEDED(sc->SetFullscreenState(FALSE, nullptr));
					}
				}
			}
		}
	}

	return false;
}

void RenderDeviceD3D::resetRenderState(void)
{
	_active_context->OMSetDepthStencilState(NULL, 0);
	_active_context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	_active_context->RSSetState(NULL);
}

bool RenderDeviceD3D::isDeferred(void) const
{
	return false;
}

bool RenderDeviceD3D::isD3D(void) const
{
	return true;
}

unsigned int RenderDeviceD3D::getViewportWidth(unsigned int device, unsigned int output) const
{
	assert(_devices.size() > device && _devices[device].viewports.size() > output);
	return (unsigned int)_devices[device].viewports[output].Width;
}

unsigned int RenderDeviceD3D::getViewportHeight(unsigned int device, unsigned int output) const
{
	assert(_devices.size() > device && _devices[device].viewports.size() > output);
	return (unsigned int)_devices[device].viewports[output].Height;
}

unsigned int RenderDeviceD3D::getActiveViewportWidth(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);
	return getViewportWidth(_curr_device, _curr_output);
}

unsigned int RenderDeviceD3D::getActiveViewportHeight(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].viewports.size() > _curr_output);
	return getViewportHeight(_curr_device, _curr_output);
}

unsigned int RenderDeviceD3D::getNumOutputs(unsigned int device) const
{
	assert(_devices.size() > device);
	return static_cast<unsigned int>(_devices[device].render_targets.size());
}

unsigned int RenderDeviceD3D::getNumDevices(void) const
{
	return static_cast<unsigned int>(_devices.size());
}

IRenderTargetPtr RenderDeviceD3D::getOutputRenderTarget(unsigned int device, unsigned int output)
{
	assert(_devices.size() > device && _devices[device].gleam_rts.size() > output);
	return _devices[device].gleam_rts[output];
}

IRenderTargetPtr RenderDeviceD3D::getActiveOutputRenderTarget(void)
{
	assert(_devices.size() > _curr_device && _devices[_curr_device].gleam_rts.size() > _curr_output);
	return getOutputRenderTarget(_curr_device, _curr_output);
}

bool RenderDeviceD3D::setCurrentOutput(unsigned int output)
{
	assert(_devices[_curr_device].swap_chains.size() > output);

	_active_render_target = _devices[_curr_device].render_targets[output];
	_active_swap_chain = _devices[_curr_device].swap_chains[output];
	_active_viewport = _devices[_curr_device].viewports[output];
	_active_vsync = _devices[_curr_device].vsync[output];

	_curr_output = output;

	return true;
}

unsigned int RenderDeviceD3D::getCurrentOutput(void) const
{
	return _curr_output;
}

bool RenderDeviceD3D::setCurrentDevice(unsigned int device)
{
	assert(_devices.size() > device);

	_active_context = _devices[device].context;
	_active_device = _devices[device].device;
	_curr_device = device;

	return setCurrentOutput(0);
}

unsigned int RenderDeviceD3D::getCurrentDevice(void) const
{
	return _curr_device;
}

unsigned int RenderDeviceD3D::getDeviceForAdapter(unsigned int adapter_id) const
{
	for (unsigned int i = 0; i < _devices.size(); ++i) {
		if (_devices[i].adapter_id == adapter_id) {
			return i;
		}
	}

	return UINT_FAIL;
}

IRenderDevice* RenderDeviceD3D::createDeferredRenderDevice(void)
{
	ID3D11DeviceContext* deferred_context = nullptr;

	if (FAILED(_active_device->CreateDeferredContext(0, &deferred_context))) {
		return nullptr;
	}

	DeferredRenderDeviceD3D* deferred_render_device = (DeferredRenderDeviceD3D*)GetAllocator()->alloc(sizeof(DeferredRenderDeviceD3D));
	new (deferred_render_device) DeferredRenderDeviceD3D();

	deferred_render_device->_context.set(deferred_context);

	return deferred_render_device;
}

void RenderDeviceD3D::executeCommandList(ICommandList* command_list)
{
	assert(command_list->isD3D() && _active_context);
	CommandListD3D* cmd_list = (CommandListD3D*)command_list;
	_active_context->ExecuteCommandList(cmd_list->getCommandList(), FALSE);
}

bool RenderDeviceD3D::finishCommandList(ICommandList*)
{
	assert(0 && "Calling a deferred render device function on an immediate render device");
	return false;
}

ID3D11DeviceContext* RenderDeviceD3D::getDeviceContext(unsigned int device)
{
	assert(_devices.size() > device);
	return _devices[device].context.get();
}

ID3D11Device* RenderDeviceD3D::getDevice(unsigned int device)
{
	assert(_devices.size() > device);
	return _devices[device].device.get();
}

ID3D11DeviceContext* RenderDeviceD3D::getActiveDeviceContext(void)
{
	return _active_context.get();
}

ID3D11Device* RenderDeviceD3D::getActiveDevice(void)
{
	return _active_device.get();
}

NS_END

#endif
