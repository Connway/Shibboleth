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

#include "Gleam_IBuffer.h"

struct ID3D11Buffer;

NS_GLEAM

class Buffer final : public IBuffer
{
public:
	Buffer(void);
	~Buffer(void);

	bool init(IRenderDevice& rd, const Settings& buffer_settings) override;
	void destroy(void) override;

	bool update(IRenderDevice& rd, const void* data, size_t size, size_t offset = 0) override;
	void* map(IRenderDevice& rd, MapType map_type = MapType::Write) override;
	void unmap(IRenderDevice& rd) override;

	RendererType getRendererType(void) const override;

	ID3D11Buffer* getBuffer(void) const;

private:
	ID3D11Buffer* _buffer;
};

NS_END