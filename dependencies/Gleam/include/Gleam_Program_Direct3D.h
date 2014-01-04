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

#include "Gleam_IProgram.h"

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11HullShader;
struct ID3D11ComputeShader;

NS_GLEAM

class ProgramD3D : public IProgram
{
public:
	ProgramD3D(void);
	~ProgramD3D(void);

	INLINE bool init(void);
	void destroy(void);

	void attach(const IShader* shader);
	void detach(const IShader* shader);
	bool link(void);

	void bind(IRenderDevice& rd);
	void unbind(IRenderDevice& rd);

	bool isD3D(void) const;

private:
	ID3D11VertexShader* _shader_vertex;
	ID3D11PixelShader* _shader_pixel;
	ID3D11DomainShader* _shader_domain;
	ID3D11GeometryShader* _shader_geometry;
	ID3D11HullShader* _shader_hull;
	ID3D11ComputeShader* _shader_compute;
};

NS_END
