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

#include "Shibboleth_Array.h"
#include <Gaff_IVirtualDestructor.h>
#include <Gleam_IShader.h>
#include <Gaff_RefPtr.h>

#define TEX_LOADER_NORMALIZED 1
#define TEX_LOADER_CUBEMAP 2

namespace Gleam
{
	class IShaderResourceView;
	class IRenderDevice;
	class IRenderTarget;
	class ISamplerState;
	class IRenderState;
	class ITexture;
	class ILayout;
	class IProgram;
	class IShader;
	class IBuffer;
	class IModel;
	class IMesh;
}

NS_SHIBBOLETH

typedef Gaff::RefPtr<Gleam::IShaderResourceView> ShaderResourceViewPtr;
typedef Gaff::RefPtr<Gleam::IRenderDevice> RenderDevicePtr;
typedef Gaff::RefPtr<Gleam::IRenderTarget> RenderTargetPtr;
typedef Gaff::RefPtr<Gleam::ISamplerState> SamplerStatePtr;
typedef Gaff::RefPtr<Gleam::IRenderState> RenderStatePtr;
typedef Gaff::RefPtr<Gleam::ITexture> TexturePtr;
typedef Gaff::RefPtr<Gleam::ILayout> LayoutPtr;
typedef Gaff::RefPtr<Gleam::IProgram> ProgramPtr;
typedef Gaff::RefPtr<Gleam::IShader> ShaderPtr;
typedef Gaff::RefPtr<Gleam::IBuffer> BufferPtr;
typedef Gaff::RefPtr<Gleam::IModel> ModelPtr;
typedef Gaff::RefPtr<Gleam::IMesh> MeshPtr;

struct TextureData : public Gaff::IVirtualDestructor
{
	Array<TexturePtr> textures;
	bool normalized;
	bool cubemap;
};

struct ShaderData : public Gaff::IVirtualDestructor
{
	Array<ShaderPtr> shaders;
	Gleam::IShader::SHADER_TYPE shader_type;
};

struct ProgramData : public Gaff::IVirtualDestructor
{
	Array<ProgramPtr> programs;
};

NS_END