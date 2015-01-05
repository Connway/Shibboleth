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

#include "Gleam_String.h"
#include <Gaff_IRefCounted.h>

NS_GLEAM

class IRenderDevice;

class IShader : public Gaff::IRefCounted
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

	INLINE bool initSource(IRenderDevice& rd, GleamAString& shader_source, SHADER_TYPE shader_type)
	{
		return initSource(rd, shader_source.getBuffer(), shader_source.size(), shader_type);
	}

	INLINE bool init(IRenderDevice& rd, const GleamAString& file_path, SHADER_TYPE shader_type)
	{
		return init(rd, file_path.getBuffer(), shader_type);
	}

	INLINE bool initVertex(IRenderDevice& rd, const GleamAString& file_path)
	{
		return initVertex(rd, file_path.getBuffer());
	}

	INLINE bool initPixel(IRenderDevice& rd, const GleamAString& file_path)
	{
		return initPixel(rd, file_path.getBuffer());
	}

	INLINE bool initDomain(IRenderDevice& rd, const GleamAString& file_path)
	{
		return initDomain(rd, file_path.getBuffer());
	}

	INLINE bool initGeometry(IRenderDevice& rd, const GleamAString& file_path)
	{
		return initGeometry(rd, file_path.getBuffer());
	}

	INLINE bool initHull(IRenderDevice& rd, const GleamAString& file_path)
	{
		return initHull(rd, file_path.getBuffer());
	}

	INLINE bool initCompute(IRenderDevice& rd, const GleamAString& file_path)
	{
		return initCompute(rd, file_path.getBuffer());
	}

	INLINE bool initVertexSource(IRenderDevice& rd, const GleamAString& source)
	{
		return initVertexSource(rd, source.getBuffer());
	}

	INLINE bool initPixelSource(IRenderDevice& rd, const GleamAString& source)
	{
		return initPixelSource(rd, source.getBuffer());
	}

	INLINE bool initDomainSource(IRenderDevice& rd, const GleamAString& source)
	{
		return initDomainSource(rd, source.getBuffer());
	}

	INLINE bool initGeometrySource(IRenderDevice& rd, const GleamAString& source)
	{
		return initGeometrySource(rd, source.getBuffer());
	}

	INLINE bool initHullSource(IRenderDevice& rd, const GleamAString& source)
	{
		return initHullSource(rd, source.getBuffer());
	}

	INLINE bool initComputeSource(IRenderDevice& rd, const GleamAString& source)
	{
		return initComputeSource(rd, source.getBuffer());
	}

#ifdef _UNICODE
	INLINE bool init(IRenderDevice& rd, const GleamWString& file_path, SHADER_TYPE shader_type)
	{
		return init(rd, file_path.getBuffer(), shader_type);
	}

	INLINE bool initVertex(IRenderDevice& rd, const GleamWString& file_path)
	{
		return initVertex(rd, file_path.getBuffer());
	}

	INLINE bool initPixel(IRenderDevice& rd, const GleamWString& file_path)
	{
		return initPixel(rd, file_path.getBuffer());
	}

	INLINE bool initDomain(IRenderDevice& rd, const GleamWString& file_path)
	{
		return initDomain(rd, file_path.getBuffer());
	}

	INLINE bool initGeometry(IRenderDevice& rd, const GleamWString& file_path)
	{
		return initGeometry(rd, file_path.getBuffer());
	}

	INLINE bool initHull(IRenderDevice& rd, const GleamWString& file_path)
	{
		return initHull(rd, file_path.getBuffer());
	}

	INLINE bool initCompute(IRenderDevice& rd, const GleamWString& file_path)
	{
		return initCompute(rd, file_path.getBuffer());
	}

	virtual bool init(IRenderDevice& rd, const wchar_t* file_path, SHADER_TYPE shader_type) = 0;

	virtual bool initVertex(IRenderDevice&, const wchar_t* file_path) = 0;
	virtual bool initPixel(IRenderDevice&, const wchar_t* file_path) = 0;
	virtual bool initDomain(IRenderDevice&, const wchar_t* file_path) = 0;
	virtual bool initGeometry(IRenderDevice&, const wchar_t* file_path) = 0;
	virtual bool initHull(IRenderDevice&, const wchar_t* file_path) = 0;
	virtual bool initCompute(IRenderDevice&, const wchar_t* file_path) = 0;
#endif

	virtual bool initSource(IRenderDevice& rd, const char* shader_source, unsigned int source_size, SHADER_TYPE shader_type) = 0;
	virtual bool initSource(IRenderDevice& rd, const char* shader_source, SHADER_TYPE shader_type) = 0;
	virtual bool init(IRenderDevice& rd, const char* file_path, SHADER_TYPE shader_type) = 0;

	virtual bool initVertex(IRenderDevice&, const char* file_path) = 0;
	virtual bool initPixel(IRenderDevice&, const char* file_path) = 0;
	virtual bool initDomain(IRenderDevice&, const char* file_path) = 0;
	virtual bool initGeometry(IRenderDevice&, const char* file_path) = 0;
	virtual bool initHull(IRenderDevice&, const char* file_path) = 0;
	virtual bool initCompute(IRenderDevice&, const char* file_path) = 0;

	virtual bool initVertexSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL) = 0;
	virtual bool initPixelSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL) = 0;
	virtual bool initDomainSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL) = 0;
	virtual bool initGeometrySource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL) = 0;
	virtual bool initHullSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL) = 0;
	virtual bool initComputeSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL) = 0;

	virtual void destroy(void) = 0;

	virtual bool isD3D(void) const = 0;

	INLINE virtual SHADER_TYPE getType(void) const { return _type; }

protected:
	SHADER_TYPE _type;
};

NS_END
