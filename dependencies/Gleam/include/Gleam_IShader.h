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

#include "Gleam_RefCounted.h"
#include "Gleam_String.h"

NS_GLEAM

class IRenderDevice;

class IShader : public RefCounted
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

	virtual bool initVertex(const IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initPixel(const IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initDomain(const IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initGeometry(const IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initHull(const IRenderDevice& rd, const GleamGString& file_path) = 0;
	virtual bool initCompute(const IRenderDevice& rd, const GleamGString& file_path) = 0;

	virtual bool initVertex(const IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initPixel(const IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initDomain(const IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initGeometry(const IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initHull(const IRenderDevice&, const GChar* file_path) = 0;
	virtual bool initCompute(const IRenderDevice&, const GChar* file_path) = 0;

	virtual bool initVertexSource(const IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initPixelSource(const IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initDomainSource(const IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initGeometrySource(const IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initHullSource(const IRenderDevice& rd, const GleamAString& source) = 0;
	virtual bool initComputeSource(const IRenderDevice& rd, const GleamAString& source) = 0;

	virtual bool initVertexSource(const IRenderDevice& rd, const char* source) = 0;
	virtual bool initPixelSource(const IRenderDevice& rd, const char* source) = 0;
	virtual bool initDomainSource(const IRenderDevice& rd, const char* source) = 0;
	virtual bool initGeometrySource(const IRenderDevice& rd, const char* source) = 0;
	virtual bool initHullSource(const IRenderDevice& rd, const char* source) = 0;
	virtual bool initComputeSource(const IRenderDevice& rd, const char* source) = 0;

	virtual void destroy(void) = 0;

	virtual bool isD3D(void) const = 0;

	INLINE virtual SHADER_TYPE getType(void) const { return _type; }

protected:
	SHADER_TYPE _type;
};

NS_END
