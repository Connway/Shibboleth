/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#ifdef GLEAM_USE_D3D11

#include "Direct3D11/Gleam_RenderDevice_Direct3D11.h"
#include "Direct3D11/Gleam_RenderTarget_Direct3D11.h"
#include "Direct3D11/Gleam_CommandList_Direct3D11.h"
#include "Direct3D11/Gleam_RenderOutput_Direct3D11.h"
#include "Gleam_Global.h"
#include "Gleam_String.h"
#include "Gleam_Window.h"
#include "Gleam_IncludeGLFWNative.h"

NS_GLEAM

IRenderDevice::AdapterList RenderDevice::GetDisplayModes(void)
{
	Vector<AdapterInfo> display_info;

	IDXGIFactory6* factory = nullptr;
	IDXGIAdapter4* adapter = nullptr;
	IDXGIOutput* adapter_output = nullptr;
	DXGI_ADAPTER_DESC3 adapter_desc;

#ifdef _DEBUG
	constexpr UINT factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr UINT factory_flags = 0;
#endif

	HRESULT result = CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&factory));
	AdapterList adapters;

	if (FAILED(result)) {
		return adapters;
	}

	for (UINT i = 0; factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		AdapterInfo info;
		Gaff::COMRefPtr<IDXGIAdapter4> adapter_ptr;

		adapter_ptr.reset(adapter);

		result = adapter->GetDesc3(&adapter_desc);

		if (FAILED(result)) {
			continue;
		}

		info.memory = static_cast<int32_t>(adapter_desc.DedicatedVideoMemory / 1024) / 1024;
		wcsncpy_s(info.adapter_name, adapter_desc.Description, std::size(adapter_desc.Description));

		for (UINT j = 0; adapter->EnumOutputs(j, &adapter_output) != DXGI_ERROR_NOT_FOUND; ++j) {
			OutputInfo out_info;
			Gaff::COMRefPtr<IDXGIOutput6> output;
			UINT num_modes;

			IDXGIOutput6* temp = nullptr;
			adapter_output->QueryInterface(&temp);
			adapter_output->Release();

			output.reset(temp);

			result = temp->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes, nullptr);

			if (FAILED(result)) {
				continue;
			}

			out_info.display_mode_list.resize(num_modes);
			temp->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &num_modes, out_info.display_mode_list.data());

			// Remove all stereo displays.
			for (int32_t k = 0; k < static_cast<int32_t>(out_info.display_mode_list.size());) {
				if (out_info.display_mode_list[k].Stereo) {
					out_info.display_mode_list.erase(out_info.display_mode_list.begin() + k);
					continue;
				}

				++k;
			}

			// Remove duplicate entries. We don't care about the scaling or scanline order
			for (int32_t k = 1; k < static_cast<int32_t>(out_info.display_mode_list.size());) {
				const DXGI_MODE_DESC1& curr = out_info.display_mode_list[k];
				const DXGI_MODE_DESC1& prev = out_info.display_mode_list[k - 1];

				if (curr.Width == prev.Width && curr.Height == prev.Height &&
					curr.RefreshRate.Numerator == prev.RefreshRate.Numerator &&
					curr.RefreshRate.Denominator == prev.RefreshRate.Denominator) {

					out_info.display_mode_list.erase(out_info.display_mode_list.begin() + k);
					continue;
				}

				++k;
			}

			DXGI_OUTPUT_DESC1 out_desc;
			result = temp->GetDesc1(&out_desc);

			if (SUCCEEDED(result)) {
				MONITORINFOEX monitor_info;
				monitor_info.cbSize = sizeof(MONITORINFOEX);

				if (GetMonitorInfo(out_desc.Monitor, &monitor_info) == TRUE) {
					out_info.curr_rect = monitor_info.rcMonitor;
					out_info.is_primary = (monitor_info.dwFlags & MONITORINFOF_PRIMARY) != 0;
					wcsncpy_s(out_info.display_name, std::size(out_info.display_name), monitor_info.szDevice, std::size(monitor_info.szDevice));
				}
			}

			info.output_info.emplace_back(out_info);
		}

		display_info.emplace_back(info);
	}

	factory->Release();

	// Convert DirectX data structures into our structure
	AdapterList out(display_info.size());

	for (int32_t i = 0; i < static_cast<int32_t>(display_info.size()); ++i) {
		const AdapterInfo& adpt_info = display_info[i];
		Adapter adpt;

		const wchar_t*  src_begin = adpt_info.adapter_name;
		const wchar_t* src_end = src_begin + eastl::CharStrlen(adpt_info.adapter_name) + 1;
		char* dest_begin = adpt.adapter_name;
		char* dest_end = dest_begin + std::size(adpt.adapter_name);

		eastl::DecodePart(src_begin, src_end, dest_begin, dest_end);

		adpt.displays.reserve(adpt_info.output_info.size());
		adpt.memory = adpt_info.memory;
		adpt.id = i;

		for (int32_t j = 0; j < static_cast<int32_t>(adpt_info.output_info.size()); ++j) {
			const OutputInfo& out_info = adpt_info.output_info[j];
			Display display;
			display.display_modes.reserve(out_info.display_mode_list.size());
			display.id = j;

			display.curr_x = out_info.curr_rect.left;
			display.curr_y = out_info.curr_rect.top;
			display.curr_width = out_info.curr_rect.right - out_info.curr_rect.left;
			display.curr_height = out_info.curr_rect.bottom - out_info.curr_rect.top;
			display.is_primary = out_info.is_primary;

			src_begin = out_info.display_name;
			src_end = src_begin + eastl::CharStrlen(out_info.display_name) + 1;
			dest_begin = display.display_name;
			dest_end = dest_begin + std::size(display.display_name);

			eastl::DecodePart(src_begin, src_end, dest_begin, dest_end);

			for (int32_t k = 0; k < static_cast<int32_t>(out_info.display_mode_list.size()); ++k) {
				const DXGI_MODE_DESC1& mode_desc = out_info.display_mode_list[k];

				DisplayMode mode = {
					static_cast<int32_t>(mode_desc.RefreshRate.Numerator / mode_desc.RefreshRate.Denominator),
					static_cast<int32_t>(mode_desc.Width),
					static_cast<int32_t>(mode_desc.Height),
					k
				};

				display.display_modes.push_back(mode);
			}

			adpt.displays.push_back(display);
		}

		out[i] = std::move(adpt);
	}

	return out;
}

bool RenderDevice::init(const Window& window)
{
	const char* const adapter_name = glfwGetWin32Adapter(window.getGLFWWindow());
	return init(adapter_name);
}

bool RenderDevice::init(const char* adapter_name)
{
	const AdapterList adapters = GetDisplayModes();

	for (const Adapter& adapter : adapters) {
		if (!strncmp(adapter_name, adapter.adapter_name, std::size(adapter.adapter_name))) {
			const int32_t adapter_id = static_cast<int32_t>(eastl::distance(adapters.begin(), &adapter));
			return init(adapter_id);
		}
	}

	// $TODO: Log error.
	return false;
}

bool RenderDevice::init(int32_t adapter_id)
{
	IDXGIFactory6* factory = nullptr;
	IDXGIAdapter4* adapter = nullptr;

#ifdef _DEBUG
	constexpr UINT factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr UINT factory_flags = 0;
#endif

	HRESULT result = CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&factory));

	if (FAILED(result)) {
		// $TODO: Log error
		return false;
	}

	if (factory->EnumAdapterByGpuPreference(static_cast<UINT>(adapter_id), DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) == DXGI_ERROR_NOT_FOUND) {
		// $TODO: Log error
		return false;
	}

	_adapter.reset(adapter);

	ID3D11DeviceContext* context = nullptr;
	ID3D11Device* device = nullptr;

#ifdef _DEBUG
	constexpr UINT device_flags = D3D11_CREATE_DEVICE_DEBUG /*| D3D11_CREATE_DEVICE_DEBUGGABLE*/;
#else
	constexpr UINT device_flags = 0;
#endif

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_1;
	result = D3D11CreateDevice(
		_adapter.get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		0,
		device_flags,
		&feature_level, 1,
		D3D11_SDK_VERSION,
		&device,
		0,
		&context
	);

	if (FAILED(result)) {
		// $TODO: Log error
		return false;
	}

	ID3D11DeviceContext3* final_context = nullptr;
	ID3D11Device5* final_device = nullptr;

	context->QueryInterface(&final_context);
	device->QueryInterface(&final_device);

	context->Release();
	device->Release();

	_device.reset(final_device);
	_context.reset(final_context);

	_adapter_id = adapter_id;

	return true;
}

//bool RenderDevice::resize(const IWindow& window)
//{
//	const Window& wnd = static_cast<const Window&>(window);
//
//	for (int32_t i = 0; i < static_cast<int32_t>(_devices.size()); ++i) {
//		Device& device = _devices[i];
//
//		for (int32_t j = 0; j < static_cast<int32_t>(device.swap_chains.size()); ++j) {
//			Gaff::COMRefPtr<ID3D11RenderTargetView>& rtv = device.render_targets[j];
//			Gaff::COMRefPtr<IDXGISwapChain>& sc = device.swap_chains[j];
//			IRenderTargetPtr rt = device.gleam_rts[j];
//			D3D11_VIEWPORT& viewport = device.viewports[j];
//			DXGI_SWAP_CHAIN_DESC sc_desc;
//
//			if (SUCCEEDED(sc->GetDesc(&sc_desc))) {
//				if (sc_desc.OutputWindow == wnd.getHWnd()) {
//
//					rtv = nullptr;
//					rt->destroy();
//					HRESULT result = sc->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
//
//					if (FAILED(result)) {
//						return false;
//					}
//
//					ID3D11Texture2D* back_buffer_ptr;
//					result = sc->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer_ptr));
//
//					if (FAILED(result)) {
//						return false;
//					}
//
//					ID3D11RenderTargetView* render_target_view = nullptr;
//					result = device.device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);
//					back_buffer_ptr->Release();
//
//					if (FAILED(result)) {
//						return false;
//					}
//
//					viewport.Width = (float)wnd.getWidth();
//					viewport.Height = (float)wnd.getHeight();
//					rtv.reset(render_target_view);
//					static_cast<RenderTargetD3D*>(rt.get())->setRTV(render_target_view, viewport);
//
//					if (wnd.getWindowMode() == IWindow::WM_FULLSCREEN) {
//						result = sc->SetFullscreenState(TRUE, g_display_info[i].output_info[j].output.get());
//					} else {
//						result = sc->SetFullscreenState(FALSE, nullptr);
//					}
//
//					return SUCCEEDED(result);
//				}
//			}
//		}
//	}
//
//	return false;
//}
//
//bool RenderDevice::handleFocusGained(const IWindow& window)
//{
//	const Window& wnd = static_cast<const Window&>(window);
//
//	for (int32_t i = 0; i < static_cast<int32_t>(_devices.size()); ++i) {
//		Device& device = _devices[i];
//
//		for (int32_t j = 0; j < static_cast<int32_t>(device.swap_chains.size()); ++j) {
//			Gaff::COMRefPtr<IDXGISwapChain>& sc = device.swap_chains[j];
//			DXGI_SWAP_CHAIN_DESC sc_desc;
//
//			if (SUCCEEDED(sc->GetDesc(&sc_desc))) {
//				if (sc_desc.OutputWindow == wnd.getHWnd()) {
//					if (wnd.getWindowMode() == IWindow::WM_FULLSCREEN) {
//						return SUCCEEDED(sc->SetFullscreenState(TRUE, g_display_info[i].output_info[j].output.get()));
//					} else {
//						return SUCCEEDED(sc->SetFullscreenState(FALSE, nullptr));
//					}
//				}
//			}
//		}
//	}
//
//	return false;
//}

IRenderDevice* RenderDevice::getOwningDevice(void) const
{
	return _owner;
}

bool RenderDevice::isDeferred(void) const
{
	return _owner != nullptr;
}

RendererType RenderDevice::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

IRenderDevice* RenderDevice::createDeferredRenderDevice(void)
{
	ID3D11DeviceContext3* deferred_context = nullptr;

	if (FAILED(_device->CreateDeferredContext3(0, &deferred_context))) {
		return nullptr;
	}

	RenderDevice* deferred_render_device = GLEAM_ALLOCT(RenderDevice);
	deferred_render_device->_context.reset(deferred_context);
	deferred_render_device->_device = _device;
	deferred_render_device->_adapter = _adapter;
	deferred_render_device->_owner = this;

	return deferred_render_device;
}

void RenderDevice::executeCommandList(ICommandList& command_list)
{
	GAFF_ASSERT(command_list.getRendererType() == RendererType::Direct3D11 && _context);
	CommandList& cmd_list = static_cast<CommandList&>(command_list);
	GAFF_ASSERT(cmd_list.getCommandList());
	_context->ExecuteCommandList(cmd_list.getCommandList(), FALSE);
}

bool RenderDevice::finishCommandList(ICommandList& command_list)
{
	GAFF_ASSERT(isDeferred() && command_list.getRendererType() == RendererType::Direct3D11 && _context);

	CommandList& cmd_list = static_cast<CommandList&>(command_list);
	ID3D11CommandList* cl = nullptr;

	const HRESULT result = _context->FinishCommandList(FALSE, &cl);

	if (FAILED(result)) {
		cmd_list.setCommandList(nullptr);
		return false;
	}

	cmd_list.setCommandList(cl);
	return true;
}

void RenderDevice::clearRenderState(void)
{
	_context->ClearState();
}

void RenderDevice::renderLineNoVertexInputInstanced(int32_t instance_count)
{
	_context->IASetInputLayout(NULL);
	_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	_context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	_context->DrawInstanced(static_cast<UINT>(2), static_cast<UINT>(instance_count), 0, 0);
}

void RenderDevice::renderLineNoVertexInput(void)
{
	_context->IASetInputLayout(NULL);
	_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	_context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	_context->Draw(2, 0);
}

void RenderDevice::renderNoVertexInput(int32_t vert_count)
{
	_context->IASetInputLayout(NULL);
	_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	_context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->Draw(static_cast<UINT>(vert_count), 0);
}

void RenderDevice::setScissorRect(const IVec2& pos, const IVec2& size)
{
	const D3D11_RECT rect = {
		pos.x,
		pos.y,
		pos.x + size.x,
		pos.y + size.y
	};

	_context->RSSetScissorRects(1, &rect);
}

void RenderDevice::setScissorRect(const IVec4& rect)
{
	const D3D11_RECT d3d_rect = {
		rect.x,
		rect.y,
		rect.z,
		rect.w
	};

	_context->RSSetScissorRects(1, &d3d_rect);
}

bool RenderDevice::isUsedBy(const Window& window) const
{
	const char* const window_adapter_name = glfwGetWin32Adapter(window.getGLFWWindow());
	const Gaff::Hash32 window_adapter_hash = Gaff::FNV1aHash32String(window_adapter_name);

	DXGI_ADAPTER_DESC3 adapter_desc;
	const HRESULT result = adapter->GetDesc3(&adapter_desc);

	if (FAILED(result)) {
		return false;
	}

	CONVERT_STRING(char8_t, adapter_name, adapter_desc.Description);
	const Gaff::Hash32 adapter_hash = Gaff::FNV1aHash32Const(adapter_name);

	return window_adapter_hash == adapter_hash;
}

ID3D11DeviceContext3* RenderDevice::getDeviceContext(void)
{
	return _context.get();
}

ID3D11Device5* RenderDevice::getDevice(void)
{
	return _device.get();
}

IDXGIAdapter4* RenderDevice::getAdapter(void)
{
	return _adapter.get();
}

NS_END

#endif
