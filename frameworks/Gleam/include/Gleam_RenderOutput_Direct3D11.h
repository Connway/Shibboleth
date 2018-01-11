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

#pragma once

#include "Gleam_RenderTarget_Direct3D11.h"
#include "Gleam_IRenderOutput.h"

struct IDXGISwapChain1;

NS_GLEAM

class RenderOutputD3D11 : public IRenderOutput
{
public:
	bool init(IRenderDevice& device, const IWindow& window, int32_t output_id, int32_t mode_id, bool vsync) override;
	bool init(IRenderDevice& device, const IWindow& window, int32_t output_id, bool vsync) override;

	RendererType getRendererType(void) const override;

	Gaff::COMRefPtr<IDXGISwapChain1>& getSwapChain(void);
	D3D11_VIEWPORT getViewport(void) const;
	bool isVSync(void) const;

private:
	Gaff::RefPtr<RenderTargetD3D11> _render_target;

	Gaff::COMRefPtr<IDXGISwapChain1> _swap_chain;
	Gaff::COMRefPtr<ID3D11DeviceContext> _context;
	Gaff::COMRefPtr<ID3D11Device> _device;

	Gaff::COMRefPtr<ID3D11RenderTargetView> _render_target_view;
	Gaff::COMRefPtr<IDXGIOutput> _output;
	bool _vsync;
};

NS_END
