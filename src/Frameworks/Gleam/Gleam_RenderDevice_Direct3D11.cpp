/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_RenderTarget_Direct3D11.h"
#include "Gleam_CommandList_Direct3D11.h"
#include "Gleam_RenderOutput_Direct3D11.h"
#include "Gleam_Window_Windows.h"
#include "Gleam_Global.h"

//#include "Gleam_IncludeD3D11.h"

NS_GLEAM

Vector<RenderDeviceD3D11::AdapterInfo> RenderDeviceD3D11::g_display_info;

template <>
IRenderDevice::AdapterList GetDisplayModes<RENDERER_DIRECT3D11>(void)
{
	if (RenderDeviceD3D11::g_display_info.empty()) {
		IDXGIFactory2* factory = nullptr;
		IDXGIAdapter1* adapter = nullptr;
		IDXGIOutput* adapter_output = nullptr;
		DXGI_ADAPTER_DESC adapter_desc;

		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&factory));
		IRenderDevice::AdapterList adapters;

		if (FAILED(result)) {
			return adapters;
		}

		for (DWORD i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
			RenderDeviceD3D11::AdapterInfo info;
			Gaff::COMRefPtr<IDXGIAdapter> adapter_ptr;

			adapter_ptr.attach(adapter);

			result = adapter->GetDesc(&adapter_desc);

			if (FAILED(result)) {
				continue;
			}

			info.memory = static_cast<UINT>(adapter_desc.DedicatedVideoMemory / 1024) / 1024;
			wcsncpy_s(info.adapter_name, 128, adapter_desc.Description, 128);

			for (DWORD j = 0; adapter->EnumOutputs(j, &adapter_output) != DXGI_ERROR_NOT_FOUND; ++j) {
				RenderDeviceD3D11::OutputInfo out_info;
				Gaff::COMRefPtr<IDXGIOutput> output;
				UINT num_modes;

				output.attach(adapter_output);
				//out_info.output.attach(adapter_output);

				result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0UL, &num_modes, nullptr);

				if (FAILED(result)) {
					continue;
				}

				out_info.display_mode_list.resize(num_modes);
				adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0UL, &num_modes, out_info.display_mode_list.data());

				// Remove duplicate entries. We don't care about the scaling or scanline order
				for (int32_t k = 1; k < static_cast<int32_t>(out_info.display_mode_list.size());) {
					const DXGI_MODE_DESC& curr = out_info.display_mode_list[k];
					const DXGI_MODE_DESC& prev = out_info.display_mode_list[k - 1];

					if (curr.Width == prev.Width && curr.Height == prev.Height &&
						curr.RefreshRate.Numerator == prev.RefreshRate.Numerator &&
						curr.RefreshRate.Denominator == prev.RefreshRate.Denominator) {

						out_info.display_mode_list.erase(out_info.display_mode_list.begin() + k);

					} else {
						++k;
					}
				}

				info.output_info.emplace_back(out_info);
			}

			RenderDeviceD3D11::g_display_info.emplace_back(info);
		}

		factory->Release();
	}

	// Convert DirectX data structures into our structure
	IRenderDevice::AdapterList out(RenderDeviceD3D11::g_display_info.size());

	for (int32_t i = 0; i < static_cast<int32_t>(RenderDeviceD3D11::g_display_info.size()); ++i) {
		const RenderDeviceD3D11::AdapterInfo& adpt_info = RenderDeviceD3D11::g_display_info[i];
		IRenderDevice::Adapter adpt;

		const wchar_t*  src_begin = adpt_info.adapter_name;
		const wchar_t* src_end = src_begin + eastl::CharStrlen(adpt_info.adapter_name);
		char* dest_begin = adpt.adapter_name;
		char* dest_end = adpt.adapter_name + 128;

		eastl::DecodePart(src_begin, src_end, dest_begin, dest_end);

		adpt.displays.reserve(adpt_info.output_info.size());
		adpt.memory = adpt_info.memory;
		adpt.id = i;

		for (int32_t j = 0; j < static_cast<int32_t>(adpt_info.output_info.size()); ++j) {
			const RenderDeviceD3D11::OutputInfo& out_info = adpt_info.output_info[j];
			IRenderDevice::Display display;
			display.display_modes.reserve(out_info.display_mode_list.size());
			display.id = j;

			for (int32_t k = 0; k < static_cast<int32_t>(out_info.display_mode_list.size()); ++k) {
				const DXGI_MODE_DESC& mode_desc = out_info.display_mode_list[k];

				IRenderDevice::DisplayMode mode = {
					static_cast<int32_t>(mode_desc.RefreshRate.Numerator / mode_desc.RefreshRate.Denominator),
					static_cast<int32_t>(mode_desc.Width),
					static_cast<int32_t>(mode_desc.Height),
					k,
					0, 0
				};

				display.display_modes.push_back(mode);
			}

			adpt.displays.push_back(display);
		}

		out.push_back(adpt);
	}

	return out;
}

bool RenderDeviceD3D11::init(int32_t adapter_id)
{
	GAFF_ASSERT(g_display_info.size() > adapter_id);

	IDXGIFactory2* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&factory));

	if (FAILED(result)) {
		// Log error
		return false;
	}

	if (factory->EnumAdapters1(static_cast<DWORD>(adapter_id), &adapter) == DXGI_ERROR_NOT_FOUND) {
		// Log error
		return false;
	}

	_adapter.attach(adapter);

	ID3D11DeviceContext* context = nullptr;
	ID3D11Device* device = nullptr;

	// Apparently trying to make the device as debug/debuggable causes it to fail. :/
//#ifdef _DEBUG
//	UINT flags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE;
//#else
	UINT flags = 0;
//#endif

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_1;
	result = D3D11CreateDevice(_adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, 0, flags, &feature_level, 1, D3D11_SDK_VERSION, &device, 0, &context);

	if (FAILED(result)) {
		// Log error
		return false;
	}

	_device.attach(device);
	_context.attach(context);

	return true;

	//const DXGI_MODE_DESC& mode = adapter.output_info[display_id].display_mode_list[display_mode_id];

	//DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	//ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	//swap_chain_desc.BufferCount = 2; // Double-buffering
	//swap_chain_desc.BufferDesc = mode;
	//swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//swap_chain_desc.OutputWindow = wnd.getHWnd();
	//swap_chain_desc.SampleDesc.Count = 1;
	//swap_chain_desc.SampleDesc.Quality = 0;
	//swap_chain_desc.Windowed = TRUE;
	//swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//if (!vsync) {
	//	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
	//	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
	//}

	//auto it = eastl::find(_devices.begin(), _devices.end(), adapter_id, [](const Device& lhs, int32_t rhs) -> bool
	//{
	//	return lhs.adapter_id == rhs;
	//});

	// We didn't find the device, so just make it and add it to the list
//	if (it == _devices.end())
//	{
//		ID3D11DeviceContext* device_context = nullptr;
//		IDXGISwapChain* swap_chain = nullptr;
//		ID3D11Device* device = nullptr;
//
//// Apparently trying to make the device as debug/debuggable causes it to fail. :/
////#ifdef _DEBUG
////		UINT flags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE;
////#else
//		UINT flags = 0;
////#endif
//
//		D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
//		HRESULT result = D3D11CreateDeviceAndSwapChain(adapter.adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, 0, flags, &feature_level, 1, D3D11_SDK_VERSION,
//														&swap_chain_desc, &swap_chain, &device, 0, &device_context);
//
//		RETURNIFFAILED(result)
//
//		ID3D11Texture2D* back_buffer_ptr = nullptr;
//		result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer_ptr));
//
//		if (FAILED(result)) {
//			swap_chain->Release();
//			device_context->Release();
//			device->Release();
//			return false;
//		}
//
//		ID3D11RenderTargetView* render_target_view = nullptr;
//		result = device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);
//
//		back_buffer_ptr->Release();
//
//		if (FAILED(result)) {
//			swap_chain->Release();
//			device_context->Release();
//			device->Release();
//			return false;
//		}
//
//		if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
//			result = swap_chain->SetFullscreenState(TRUE, g_display_info[adapter_id].output_info[display_id].output.get());
//		} else {
//			result = swap_chain->SetFullscreenState(FALSE, nullptr);
//		}
//
//		Gaff::COMRefPtr<ID3D11RenderTargetView> rtv;
//		Gaff::COMRefPtr<IDXGISwapChain> sc;
//		rtv.attach(render_target_view);
//		sc.attach(swap_chain);
//
//		RETURNIFFAILED(result)
//
//		D3D11_VIEWPORT viewport;
//		viewport.Width = static_cast<float>(wnd.getWidth());
//		viewport.Height = static_cast<float>(wnd.getHeight());
//		viewport.MinDepth = 0.0f;
//		viewport.MaxDepth = 1.0f;
//		viewport.TopLeftX = 0.0f;
//		viewport.TopLeftY = 0.0f;
//
//		Device dvc;
//		dvc.render_targets.push_back(rtv);
//		dvc.swap_chains.push_back(sc);
//		dvc.viewports.push_back(viewport);
//		dvc.context.attach(device_context);
//		dvc.device.attach(device);
//		dvc.vsync.push_back(vsync);
//		dvc.adapter_id = adapter_id;
//
//		RenderTargetD3D* rt = GleamAllocateT(RenderTargetD3D);
//		rt->setRTV(rtv.get(), viewport);
//
//		dvc.gleam_rts.emplace_back(rt);
//
//		_devices.push_back(dvc);
//
//	// We found the device already made, so make a new swap chain and render target and add them to the list
//	} else {
//		IDXGISwapChain* swap_chain = nullptr;
//		IDXGIFactory* factory = nullptr;
//
//		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
//		RETURNIFFAILED(result)
//
//		result = factory->CreateSwapChain(it->device.get(), &swap_chain_desc, &swap_chain);
//
//		factory->Release();
//		RETURNIFFAILED(result)
//
//		ID3D11Texture2D* back_buffer_ptr = nullptr;
//		result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer_ptr);
//
//		if (FAILED(result)) {
//			swap_chain->Release();
//			return false;
//		}
//
//		ID3D11RenderTargetView* render_target_view = nullptr;
//		result = it->device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);
//
//		back_buffer_ptr->Release();
//
//		if (FAILED(result)) {
//			swap_chain->Release();
//			return false;
//		}
//
//		if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
//			result = swap_chain->SetFullscreenState(TRUE, g_display_info[adapter_id].output_info[display_id].output.get());
//		} else {
//			result = swap_chain->SetFullscreenState(FALSE, nullptr);
//		}
//
//		Gaff::COMRefPtr<ID3D11RenderTargetView> rtv;
//		Gaff::COMRefPtr<IDXGISwapChain> sc;
//		rtv.attach(render_target_view);
//		sc.attach(swap_chain);
//
//		RETURNIFFAILED(result)
//
//		it->render_targets.push_back(rtv);
//		it->swap_chains.push_back(sc);
//		it->vsync.push_back(vsync);
//
//		D3D11_VIEWPORT viewport;
//		viewport.Width = static_cast<float>(wnd.getWidth());
//		viewport.Height = static_cast<float>(wnd.getHeight());
//		viewport.MinDepth = 0.0f;
//		viewport.MaxDepth = 1.0f;
//		viewport.TopLeftX = 0.0f;
//		viewport.TopLeftY = 0.0f;
//
//		it->viewports.push_back(viewport);
//
//		RenderTargetD3D* rt = GleamAllocateT(RenderTargetD3D);
//		rt->setRTV(rtv.get(), viewport);
//
//		it->gleam_rts.push_back(IRenderTargetPtr(rt));
//	}
//
//	if (!_active_device) {
//		setCurrentDevice(0);
//	}
//
//	return true;
}

void RenderDeviceD3D11::frameBegin(IRenderOutput& output)
{
	GAFF_ASSERT(output.getRendererType() == RENDERER_DIRECT3D11);
	RenderOutputD3D11& out = reinterpret_cast<RenderOutputD3D11&>(output);
	D3D11_VIEWPORT viewport = out.getViewport();

	resetRenderState();
	_context->RSSetViewports(1, &viewport);
}

void RenderDeviceD3D11::frameEnd(IRenderOutput& output)
{
	GAFF_ASSERT(output.getRendererType() == RENDERER_DIRECT3D11);
	RenderOutputD3D11& out = reinterpret_cast<RenderOutputD3D11&>(output);
	out.getSwapChain()->Present(out.isVSync(), 0);
}

//void RenderDeviceD3D11::beginFrame(void)
//{
//	resetRenderState();
//	_active_context->RSSetViewports(1, &_active_viewport);
//}

//void RenderDeviceD3D11::endFrame(void)
//{
//	_active_swap_chain->Present(_active_vsync, 0);
//}

//bool RenderDeviceD3D11::resize(const IWindow& window)
//{
//	const Window& wnd = reinterpret_cast<const Window&>(window);
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
//					RETURNIFFAILED(result)
//
//					ID3D11Texture2D* back_buffer_ptr;
//					result = sc->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer_ptr));
//					RETURNIFFAILED(result)
//
//					ID3D11RenderTargetView* render_target_view = nullptr;
//					result = device.device->CreateRenderTargetView(back_buffer_ptr, nullptr, &render_target_view);
//					back_buffer_ptr->Release();
//					RETURNIFFAILED(result)
//
//					viewport.Width = (float)wnd.getWidth();
//					viewport.Height = (float)wnd.getHeight();
//					rtv.attach(render_target_view);
//					reinterpret_cast<RenderTargetD3D*>(rt.get())->setRTV(render_target_view, viewport);
//
//					if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
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
//bool RenderDeviceD3D11::handleFocusGained(const IWindow& window)
//{
//	const Window& wnd = reinterpret_cast<const Window&>(window);
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
//					if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
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

bool RenderDeviceD3D11::isDeferred(void) const
{
	return _is_deferred;
}

RendererType RenderDeviceD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
}

IRenderDevice* RenderDeviceD3D11::createDeferredRenderDevice(void)
{
	ID3D11DeviceContext* deferred_context = nullptr;

	if (FAILED(_device->CreateDeferredContext(0, &deferred_context))) {
		return nullptr;
	}

	RenderDeviceD3D11* deferred_render_device = GLEAM_ALLOCT(RenderDeviceD3D11);
	deferred_render_device->_context.attach(deferred_context);
	deferred_render_device->_device = _device;
	deferred_render_device->_adapter = _adapter;
	deferred_render_device->_is_deferred = true;

	return deferred_render_device;
}

void RenderDeviceD3D11::executeCommandList(ICommandList* command_list)
{
	GAFF_ASSERT(command_list->getRendererType() == RENDERER_DIRECT3D11 && _context);
	CommandListD3D11* cmd_list = reinterpret_cast<CommandListD3D11*>(command_list);
	GAFF_ASSERT(cmd_list->getCommandList());
	_context->ExecuteCommandList(cmd_list->getCommandList(), FALSE);
}

bool RenderDeviceD3D11::finishCommandList(ICommandList* command_list)
{
	GAFF_ASSERT(_is_deferred && command_list->getRendererType() == RENDERER_DIRECT3D11 && _context);

	CommandListD3D11* cmd_list = reinterpret_cast<CommandListD3D11*>(command_list);
	ID3D11CommandList* cl = nullptr;

	if (FAILED(_context->FinishCommandList(FALSE, &cl))) {
		return false;
	}

	cmd_list->setCommandList(cl);
	return true;
}

void RenderDeviceD3D11::resetRenderState(void)
{
	_context->OMSetDepthStencilState(NULL, 0);
	_context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	_context->RSSetState(NULL);
}

void RenderDeviceD3D11::renderNoVertexInput(int32_t vert_count)
{
	_context->IASetInputLayout(NULL);
	_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	_context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->Draw(vert_count, 0);
}

ID3D11DeviceContext* RenderDeviceD3D11::getDeviceContext(void)
{
	return _context.get();
}

ID3D11Device* RenderDeviceD3D11::getDevice(void)
{
	return _device.get();
}

IDXGIAdapter1* RenderDeviceD3D11::getAdapter(void)
{
	return _adapter.get();
}

NS_END

#endif
