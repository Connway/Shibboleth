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

#include "Gleam_RenderOutput_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Window.h"
#include <Gaff_Assert.h>

NS_GLEAM

bool RenderOutputD3D11::init(IRenderDevice& device, const IWindow& window, int32_t output_id, bool vsync)
{
	GAFF_ASSERT(device.getRendererType() == RENDERER_DIRECT3D11);
	// If fullscreen, find closest display mode.
	// Else, use width/height values.

	GAFF_REF(window);
	GAFF_REF(output_id);
	GAFF_REF(vsync);

	return true;
}

// Axe mode_id?
bool RenderOutputD3D11::init(IRenderDevice& device, const IWindow& window, int32_t output_id, int32_t /*mode_id*/, bool vsync)
{
	GAFF_ASSERT(device.getRendererType() == RENDERER_DIRECT3D11);
	_vsync = vsync;

	RenderDeviceD3D11& dvc = reinterpret_cast<RenderDeviceD3D11&>(device);
	const Window& wnd = reinterpret_cast<const Window&>(window);

	IDXGISwapChain1* swap_chain = nullptr;
	IDXGIFactory6* factory = nullptr;
	
//#ifdef _DEBUG
//	constexpr UINT factory_flags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE;
//#else
	constexpr UINT factory_flags = 0;
//#endif

	HRESULT result = CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&factory));

	if (FAILED(result)) {
		// Log error
		return false;
	}

	IDXGIOutput* adapter_output = nullptr;

	if (dvc.getAdapter()->EnumOutputs(static_cast<DWORD>(output_id), &adapter_output) == DXGI_ERROR_NOT_FOUND) {
		// Log error
		factory->Release();
		return false;
	}

	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
	swap_chain_desc.Width = 0;
	swap_chain_desc.Height = 0;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.Stereo = FALSE;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 2; // Double-buffering
	swap_chain_desc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (wnd.getWindowMode() == IWindow::FULLSCREEN) {
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc;
		swap_chain_fullscreen_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_chain_fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_fullscreen_desc.Windowed = wnd.isFullScreen() ? FALSE : TRUE;

		if (!vsync) {
			swap_chain_fullscreen_desc.RefreshRate.Numerator = 0;
			swap_chain_fullscreen_desc.RefreshRate.Denominator = 0;
		}

		result = factory->CreateSwapChainForHwnd(dvc.getDevice(), wnd.getHWnd(), &swap_chain_desc, &swap_chain_fullscreen_desc, adapter_output, &swap_chain);

	} else {
		result = factory->CreateSwapChainForHwnd(dvc.getDevice(), wnd.getHWnd(), &swap_chain_desc, NULL, NULL, &swap_chain);
	}

	factory->Release();

	if (FAILED(result)) {
		// Log error
		adapter_output->Release();
		swap_chain->Release();
		return false;
	}

	IDXGISwapChain4* final_swap_chain = nullptr;
	swap_chain->QueryInterface(&final_swap_chain);

	swap_chain->Release();

	if (window.getWindowMode() == IWindow::FULLSCREEN) {
		result = final_swap_chain->SetFullscreenState(TRUE, adapter_output);
	} else {
		result = final_swap_chain->SetFullscreenState(FALSE, NULL);
	}

	if (FAILED(result)) {
		// Log error
		final_swap_chain->Release();
		adapter_output->Release();
		return false;
	}

	ID3D11Texture2D* back_buffer_ptr = nullptr;
	result = final_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer_ptr));
	
	if (FAILED(result)) {
		// Log error
		final_swap_chain->Release();
		adapter_output->Release();
		return false;
	}

	ID3D11RenderTargetView1* render_target_view = nullptr;
	result = dvc.getDevice()->CreateRenderTargetView1(back_buffer_ptr, nullptr, &render_target_view);
	
	back_buffer_ptr->Release();
	
	if (FAILED(result)) {
		final_swap_chain->Release();
		adapter_output->Release();
		return false;
	}

	if (FAILED(result)) {
		// Log error
		render_target_view->Release();
		final_swap_chain->Release();
		adapter_output->Release();
		return false;
	}

	IDXGIOutput6* final_output = nullptr;
	adapter_output->QueryInterface(&final_output);

	adapter_output->Release();

	_render_target_view.attach(render_target_view);
	_swap_chain.attach(final_swap_chain);
	_output.attach(final_output);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(wnd.getWidth());
	viewport.Height = static_cast<float>(wnd.getHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
		
	RenderTargetD3D11* rt = GLEAM_ALLOCT(RenderTargetD3D11);
	rt->setRTV(_render_target_view.get(), viewport);
	_render_target = rt;
	
	return true;
}

RendererType RenderOutputD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
}

Gaff::COMRefPtr<IDXGISwapChain4>& RenderOutputD3D11::getSwapChain(void)
{
	return _swap_chain;
}

D3D11_VIEWPORT RenderOutputD3D11::getViewport(void) const
{
	return _render_target->getViewport();
}

bool RenderOutputD3D11::isVSync(void) const
{
	return _vsync;
}

NS_END
