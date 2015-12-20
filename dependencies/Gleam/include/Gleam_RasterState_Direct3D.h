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

#pragma once

#include "Gleam_IRasterState.h"
#include "Gleam_RefCounted.h"

struct ID3D11RasterizerState;

NS_GLEAM

class RasterStateD3D : public IRasterState
{
public:
	RasterStateD3D(void);
	~RasterStateD3D(void);

	bool init(IRenderDevice& rd, const RasterStateSettings& settings) override;
	void destroy(void) override;

	void set(IRenderDevice& rd) const override;
	void unset(IRenderDevice& rd) const override;

	bool isD3D(void) const override;

private:
	ID3D11RasterizerState* _raster_state;

	GLEAM_REF_COUNTED(RasterStateD3D);
};

NS_END
