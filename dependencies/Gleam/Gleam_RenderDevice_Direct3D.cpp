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

#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_Buffer_Direct3D.h"
#include "Gleam_Window_Windows.h"
#include "Gaff_IncludeAssert.h"
#include "Gleam_Global.h"

NS_GLEAM

RenderDeviceD3D::RenderDeviceD3D(void):
	_vsync(false), _display_mode_list(NULLPTR),
	_render_target_view(NULLPTR), _device_context(NULLPTR),
	_swap_chain(NULLPTR), _device(NULLPTR)
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

bool RenderDeviceD3D::init(const Window& window, bool vsync, int color_format, unsigned int flags)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	unsigned int numerator, denominator;
	HRESULT result;

	_vsync = vsync;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	RETURNIFFAILED(result)

	result = factory->EnumAdapters(0, &adapter);
	RETURNIFFAILED(result)

	result = adapter->EnumOutputs(0, &adapterOutput);
	RETURNIFFAILED(result)

	result = adapterOutput->GetDisplayModeList((DXGI_FORMAT)color_format, flags, &_num_modes, NULL);
	RETURNIFFAILED(result)

	_display_mode_list = (DXGI_MODE_DESC*)GleamAllocate(sizeof(DXGI_MODE_DESC) * _num_modes);
	if (!_display_mode_list) {
		return false;
	}

	result = adapterOutput->GetDisplayModeList((DXGI_FORMAT)color_format, flags, &_num_modes, _display_mode_list);
	RETURNIFFAILED(result)

	numerator = 0;
	denominator = 0;

	bool found = false;
	for (UINT i = 0; i < _num_modes; ++i) {
		if (_display_mode_list[i].Width == window.getWidth() && _display_mode_list[i].Height == window.getHeight()) {
			numerator = _display_mode_list[i].RefreshRate.Numerator;
			denominator = _display_mode_list[i].RefreshRate.Denominator;
			found = true;
			break;
		}
	}

	if (!found) {
		return false;
	}

	result = adapter->GetDesc(&adapterDesc);
	RETURNIFFAILED(result)

	_video_memory = (UINT)adapterDesc.DedicatedVideoMemory;
	wcsncpy_s(_video_card_name, 128, adapterDesc.Description, 128);

	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = window.getWidth();
	swapChainDesc.BufferDesc.Height = window.getHeight();
	swapChainDesc.BufferDesc.Format = (DXGI_FORMAT)color_format;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window.getHWnd();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !window.isFullScreen();
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (vsync) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	} else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	featureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION,
											&swapChainDesc, &_swap_chain, &_device, 0, &_device_context);
	RETURNIFFAILED(result)

	result = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	RETURNIFFAILED(result)

	result = _device->CreateRenderTargetView(backBufferPtr, 0, &_render_target_view);
	RETURNIFFAILED(result)

	backBufferPtr->Release();

	_viewport.Width = (float)window.getWidth();
	_viewport.Height = (float)window.getHeight();
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	return true;
}

void RenderDeviceD3D::destroy(void)
{
	if (_swap_chain) {
		_swap_chain->SetFullscreenState(false, NULLPTR);
	}

	if (_render_target_view) {
		_render_target_view->Release();
		_render_target_view = NULLPTR;
	}

	if (_device_context) {
		_device_context->Release();
		_device_context = NULLPTR;
	}

	if (_device) {
		_device->Release();
		_device = NULLPTR;
	}

	if (_swap_chain) {
		_swap_chain->Release();
		_swap_chain = NULLPTR;
	}

	if (_display_mode_list) {
		GleamFree(_display_mode_list);
		_display_mode_list = NULLPTR;
	}
}

bool RenderDeviceD3D::isVsync(void) const
{
	return _vsync;
}

bool RenderDeviceD3D::setVsync(bool vsync)
{
	_vsync = vsync;
	return createSwapChain();
}

void RenderDeviceD3D::setClearColor(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;
}

void RenderDeviceD3D::beginFrame(void)
{
	resetRenderState();
	_device_context->ClearRenderTargetView(_render_target_view, _clear_color);
}

void RenderDeviceD3D::endFrame(void)
{
	_swap_chain->Present((_vsync) ? 1 : 0, 0);
}

bool RenderDeviceD3D::resize(const Window& window)
{
	_render_target_view->Release();
	_render_target_view = NULLPTR;

	HRESULT result = _swap_chain->ResizeBuffers(0, window.getWidth(), window.getHeight(), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	RETURNIFFAILED(result)

	ID3D11Texture2D* backBufferPtr;
	result = _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	RETURNIFFAILED(result)

	result = _device->CreateRenderTargetView(backBufferPtr, 0, &_render_target_view);
	RETURNIFFAILED(result)

	backBufferPtr->Release();

	_viewport.Width = (float)window.getWidth();
	_viewport.Height = (float)window.getHeight();
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	result = _swap_chain->SetFullscreenState(window.getWindowMode() == Window::FULLSCREEN, NULLPTR);
	return SUCCEEDED(result);
}

void RenderDeviceD3D::unbindRenderTarget(void)
{
	_device_context->OMSetRenderTargets(1, &_render_target_view, NULL);
	_prev_rt = NULLPTR;
}

void RenderDeviceD3D::resetRenderState(void)
{
	_device_context->OMSetDepthStencilState(NULL, 0);
	_device_context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	_device_context->RSSetState(NULL);
	_device_context->RSSetViewports(1, &_viewport);
}

bool RenderDeviceD3D::isD3D(void) const
{
	return true;
}

unsigned int RenderDeviceD3D::getViewportWidth(void) const
{
	return (unsigned int)_viewport.Width;
}

unsigned int RenderDeviceD3D::getViewportHeight(void) const
{
	return (unsigned int)_viewport.Height;
}

ID3D11DeviceContext* RenderDeviceD3D::getDeviceContext(void) const
{
	return _device_context;
}

ID3D11Device* RenderDeviceD3D::getDevice(void) const
{
	return _device;
}

// Unknown if this function actually works.
bool RenderDeviceD3D::createSwapChain(void)
{
	if (_swap_chain) {
		DXGI_SWAP_CHAIN_DESC swap_desc;
		HRESULT result = _swap_chain->GetDesc(&swap_desc);
		RETURNIFFAILED(result)

		if (_vsync) {
			unsigned int numerator = 0;
			unsigned int denominator = 0;

			bool found = false;
			for (UINT i = 0; i < _num_modes; ++i) {
				if (_display_mode_list[i].Width == swap_desc.BufferDesc.Width && _display_mode_list[i].Height == swap_desc.BufferDesc.Height) {
					numerator = _display_mode_list[i].RefreshRate.Numerator;
					denominator = _display_mode_list[i].RefreshRate.Denominator;
					found = true;
					break;
				}
			}

			if (!found) {
				return false;
			}

			swap_desc.BufferDesc.RefreshRate.Numerator = numerator;
			swap_desc.BufferDesc.RefreshRate.Denominator = denominator;

		} else {
			swap_desc.BufferDesc.RefreshRate.Numerator = 0;
			swap_desc.BufferDesc.RefreshRate.Denominator = 1;
		}

		IDXGIFactory* factory;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		RETURNIFFAILED(result)

		IDXGISwapChain* swap_chain = NULLPTR;
		result = factory->CreateSwapChain(_device, &swap_desc, &swap_chain);
		factory->Release();
		RETURNIFFAILED(result)

		ID3D11Texture2D* back_buffer = NULLPTR;
		result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

		if (FAILED(result)) {
			swap_chain->Release();
			return false;
		}

		ID3D11RenderTargetView* render_target_view = NULLPTR;
		result = _device->CreateRenderTargetView(back_buffer, 0, &render_target_view);

		if (FAILED(result)) {
			swap_chain->Release();
			return false;
		}

		_swap_chain->Release();
		_swap_chain = swap_chain;

		_render_target_view->Release();
		_render_target_view = render_target_view;

		back_buffer->Release();

		return true;
	}

	return false;
}

NS_END
