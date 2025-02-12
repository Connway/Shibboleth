/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Gleam_IDepthStencilState.h"

struct ID3D11DepthStencilState;

NS_GLEAM

class DepthStencilStateD3D11 : public IDepthStencilState
{
public:
	DepthStencilStateD3D11(void);
	~DepthStencilStateD3D11(void);

	bool init(IRenderDevice& rd, const Settings& settings) override;
	void destroy(void) override;

	void bind(IRenderDevice& rd) const override;
	void unbind(IRenderDevice& rd) const override;

	RendererType getRendererType(void) const override;

private:
	ID3D11DepthStencilState* _depth_stencil_state;
	unsigned int _stencil_ref;
};

NS_END
