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

#pragma once


#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include <dxgi.h>

NS_GLEAM

class RenderDeviceD3D : public IRenderDevice
{
public:
	RenderDeviceD3D(void);
	~RenderDeviceD3D(void);

	bool init(const Window& window, bool vsync = false,
				int color_format = 28, unsigned int flags = 1UL);
	void destroy(void);

	INLINE bool isVsync(void) const;
	bool setVsync(bool vsync);

	INLINE void setClearColor(float r, float g, float b, float a);

	INLINE void beginFrame(void);
	INLINE void endFrame(void);

	bool resize(const Window& window);

	INLINE void unbindRenderTarget(void);
	void resetRenderState(void);

	INLINE bool isD3D(void) const;

	INLINE unsigned int getViewportWidth(void) const;
	INLINE unsigned int getViewportHeight(void) const;

	INLINE ID3D11DeviceContext* getDeviceContext(void) const;
	INLINE ID3D11Device* getDevice(void) const;

private:
	float _clear_color[4];
	bool _vsync;

	unsigned int _num_modes;
	DXGI_MODE_DESC* _display_mode_list;
	wchar_t _video_card_name[128];
	unsigned int _video_memory;

	ID3D11RenderTargetView* _render_target_view;
	ID3D11DeviceContext* _device_context;
	IDXGISwapChain* _swap_chain;
	ID3D11Device* _device;

	D3D11_VIEWPORT _viewport;

	bool createSwapChain(void);
};

NS_END
