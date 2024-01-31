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

#pragma once

#include "Direct3D11/Gleam_RenderTarget_Direct3D11.h"
#include "Gleam_IRenderOutput.h"
#include "Gleam_SmartPtrs.h"
#include <Gaff_RefPtr.h>

struct IDXGISwapChain1;

NS_GLEAM

class RenderOutput final : public IRenderOutput
{
public:
	bool init(IRenderDevice& device, const Window& window, int32_t refresh_rate, bool vsync) override;

	RendererType getRendererType(void) const override;

	IVec2 getSize(void) const override;

	const IRenderTarget& getRenderTarget(void) const override;
	IRenderTarget& getRenderTarget(void) override;

	void present(void) override;

	const Gaff::COMRefPtr<IDXGISwapChain4>& getSwapChain(void) const;
	Gaff::COMRefPtr<IDXGISwapChain4>& getSwapChain(void);
	const ID3D11Texture2D1* getBackBufferTexture(void) const;
	ID3D11Texture2D1* getBackBufferTexture(void);

	D3D11_VIEWPORT getViewport(void) const;
	bool isVSync(void) const;

private:
	UniquePtr<RenderTarget> _render_target;

	Gaff::COMRefPtr<IDXGISwapChain4> _swap_chain;
	Gaff::COMRefPtr<ID3D11RenderTargetView1> _render_target_view;

	UINT _present_flags = 0;
	bool _vsync = false;
};

NS_END
