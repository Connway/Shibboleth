/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_ResourceDefines.h>
#include <Shibboleth_ResourceWrapper.h>
#include <Shibboleth_Array.h>

namespace Gleam
{
	class Vector4CPU;
}

NS_SHIBBOLETH

struct ModelData;

class IModelComponent
{
public:
	struct MeshData
	{
		ResourceWrapper<ProgramBuffersData> program_buffers;
		ResourceWrapper<ProgramData> material;

		Array< ResourceWrapper<TextureData> > textures;
		Array< ResourceWrapper<SamplerStateData> > samplers;
		//Array< ResourceWrapper<BufferData> > _buffers;
	};


	IModelComponent(void) {}
	virtual ~IModelComponent(void) {}

	virtual void setStatic(bool is_static) = 0;
	virtual bool isStatic(void) const = 0;

	virtual size_t determineLOD(const Gleam::Vector4CPU& pos) = 0;

	//virtual unsigned int getMeshInstanceHash(size_t mesh) const;

	virtual const Array<MeshData>& getMeshData(void) const = 0;
	virtual Array<MeshData>& getMeshData(void) = 0;

	virtual const ModelData& getModel(void) const = 0;
	virtual ModelData& getModel(void) = 0;

	SHIB_INTERFACE_REFLECTION(IModelComponent)
	SHIB_INTERFACE_NAME("Model Component")
};

NS_END
