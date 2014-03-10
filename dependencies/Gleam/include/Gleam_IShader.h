/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gleam_RefCounted.h"
#include "Gleam_String.h"

NS_GLEAM

class IRenderDevice;

class IShader : public GleamRefCounted
{
public:
	enum SHADER_TYPE
	{
		SHADER_VERTEX = 0,
		SHADER_PIXEL,
		SHADER_DOMAIN,
		SHADER_GEOMETRY,
		SHADER_HULL,
		SHADER_COMPUTE,
		SHADER_TYPE_SIZE
	};

	virtual ~IShader(void) {}

	virtual bool initVertex(IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initPixel(IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initDomain(IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initGeometry(IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initHull(IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initCompute(IRenderDevice& rd, const GleamGString& file_path) = 0;

	virtual bool initVertex(IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initPixel(IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initDomain(IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initGeometry(IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initHull(IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initCompute(IRenderDevice&, const GChar* file_path) = 0;

	virtual bool initVertexSource(IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initPixelSource(IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initDomainSource(IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initGeometrySource(IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initHullSource(IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initComputeSource(IRenderDevice& rd, const GleamAString& source) = 0;

	virtual bool initVertexSource(IRenderDevice& rd, const char* source) = 0;
	virtual bool initPixelSource(IRenderDevice& rd, const char* source) = 0;
	virtual bool initDomainSource(IRenderDevice& rd, const char* source) = 0;
	virtual bool initGeometrySource(IRenderDevice& rd, const char* source) = 0;
	virtual bool initHullSource(IRenderDevice& rd, const char* source) = 0;
	virtual bool initComputeSource(IRenderDevice& rd, const char* source) = 0;

	virtual void destroy(void) = 0;

	virtual bool isD3D(void) const = 0;

	INLINE virtual SHADER_TYPE getType(void) const { return _type; }

protected:
	SHADER_TYPE _type;
};

NS_END
