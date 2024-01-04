/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Gleam_RenderOutput_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_Window.h"
#include <Gaff_Assert.h>

NS_GLEAM

bool RenderOutput::init(IRenderDevice& device, const Window& window, int32_t display_id, int32_t refresh_rate, bool vsync)
{
	GAFF_ASSERT(device.getRendererType() == RendererType::Direct3D11);
	_vsync = vsync && !window.isFullscreen();

	RenderDevice& rd3d = static_cast<RenderDevice&>(device);
	const Window& wnd = static_cast<const Window&>(window);

	IDXGISwapChain1* swap_chain = nullptr;
	IDXGIFactory6* factory = nullptr;

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

	IDXGIOutput* adapter_output = nullptr;

	if (rd3d.getAdapter()->EnumOutputs(static_cast<DWORD>(display_id), &adapter_output) == DXGI_ERROR_NOT_FOUND) {
		// $TODO: Log error
		factory->Release();
		return false;
	}

	const IVec2& size = wnd.getSize();

	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
	swap_chain_desc.Width = static_cast<UINT>(size.x);
	swap_chain_desc.Height = static_cast<UINT>(size.y);
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.Stereo = FALSE;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 2; // Double-buffering
	swap_chain_desc.Scaling = DXGI_SCALING_NONE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	const HWND hwnd = reinterpret_cast<HWND>(wnd.getHWnd());

	if (wnd.isFullscreen()) {
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc;
		swap_chain_fullscreen_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_chain_fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_fullscreen_desc.Windowed = FALSE;

		swap_chain_fullscreen_desc.RefreshRate.Numerator = refresh_rate;
		swap_chain_fullscreen_desc.RefreshRate.Denominator = 1;


		result = factory->CreateSwapChainForHwnd(rd3d.getDevice(), hwnd, &swap_chain_desc, &swap_chain_fullscreen_desc, adapter_output, &swap_chain);
		_present_flags |= DXGI_PRESENT_RESTRICT_TO_OUTPUT;

	} else {
		result = factory->CreateSwapChainForHwnd(rd3d.getDevice(), hwnd, &swap_chain_desc, NULL, NULL, &swap_chain);
		_present_flags |= DXGI_PRESENT_ALLOW_TEARING;
	}

	factory->Release();

	if (FAILED(result)) {
		// Log error
		adapter_output->Release();
		return false;
	}

	IDXGISwapChain4* final_swap_chain = nullptr;
	swap_chain->QueryInterface(&final_swap_chain);

	swap_chain->Release();

	if (wnd.isFullscreen()) {
		result = final_swap_chain->SetFullscreenState(TRUE, adapter_output);
	} else {
		result = final_swap_chain->SetFullscreenState(FALSE, NULL);
	}

	adapter_output->Release();

	if (FAILED(result)) {
		// Log error
		final_swap_chain->Release();
		return false;
	}

	ID3D11Texture2D1* back_buffer_ptr = nullptr;
	result = final_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer_ptr));

	if (FAILED(result)) {
		// Log error
		final_swap_chain->Release();
		return false;
	}

	// Render target for outputs converts to sRGB space.
	D3D11_RENDER_TARGET_VIEW_DESC1 rtv_desc;
	rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;
	rtv_desc.Texture2D.PlaneSlice = 0;

	ID3D11RenderTargetView1* render_target_view = nullptr;
	result = rd3d.getDevice()->CreateRenderTargetView1(back_buffer_ptr, &rtv_desc, &render_target_view);

	back_buffer_ptr->Release();

	if (FAILED(result)) {
		// Log error
		final_swap_chain->Release();
		return false;
	}

	_render_target_view.reset(render_target_view);
	_swap_chain.reset(final_swap_chain);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(size.x);
	viewport.Height = static_cast<float>(size.y);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	RenderTarget* const rt = GLEAM_ALLOCT(RenderTarget);
	rt->setRTV(_render_target_view.get(), viewport);
	_render_target.reset(rt);

	return true;
}

RendererType RenderOutput::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

IVec2 RenderOutput::getSize(void) const
{
	const D3D11_VIEWPORT viewport = getViewport();

	return IVec2{
		static_cast<int32_t>(viewport.Width),
		static_cast<int32_t>(viewport.Height)
	};
}

const IRenderTarget& RenderOutput::getRenderTarget(void) const
{
	return *_render_target;
}

IRenderTarget& RenderOutput::getRenderTarget(void)
{
	return *_render_target;
}

void RenderOutput::present(void)
{
	static const DXGI_PRESENT_PARAMETERS present_params = { 0, NULL, NULL, NULL };
	const UINT interval = (_vsync) ? 1 : 0;

	_swap_chain->Present1(interval, _present_flags, &present_params);
}

const Gaff::COMRefPtr<IDXGISwapChain4>& RenderOutput::getSwapChain(void) const
{
	return _swap_chain;
}

Gaff::COMRefPtr<IDXGISwapChain4>& RenderOutput::getSwapChain(void)
{
	return _swap_chain;
}

const ID3D11Texture2D1* RenderOutput::getBackBufferTexture(void) const
{
	return const_cast<RenderOutput*>(this)->getBackBufferTexture();
}

ID3D11Texture2D1* RenderOutput::getBackBufferTexture(void)
{
	ID3D11Texture2D1* back_buffer_ptr = nullptr;
	_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer_ptr));

	return back_buffer_ptr;
}

D3D11_VIEWPORT RenderOutput::getViewport(void) const
{
	return _render_target->getViewport();
}

bool RenderOutput::isVSync(void) const
{
	return _vsync;
}

NS_END

#endif
