/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Gleam_IProgram.h"

struct ID3D11DeviceChild;

NS_GLEAM

class Shader;

class Program: public IProgram
{
public:
	void attach(IShader* shader) override;
	void detach(IShader::Type shader) override;

	void bind(IRenderDevice& rd) override;
	void unbind(IRenderDevice& rd) override;

	const IShader* getAttachedShader(IShader::Type type) const override;
	IShader* getAttachedShader(IShader::Type type) override;

	RendererType getRendererType(void) const;

private:
	Shader* _attached_shaders[static_cast<size_t>(IShader::Type::Count)] = { nullptr };
	ID3D11DeviceChild* _d3d_shaders[static_cast<size_t>(IShader::Type::Count)] = { nullptr };
};

NS_END
