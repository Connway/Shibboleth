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

#include "Shibboleth_ResourceWrapper.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_Array.h>
#include <Gaff_IVirtualDestructor.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_IShader.h>
#include <Gaff_RefPtr.h>
#include <Gaff_Image.h>

namespace Gleam
{
	class IShaderResourceView;
	class IProgramBuffers;
	class IRenderDevice;
	class IRenderTarget;
	class IRenderState;
	class ICommandList;
	class ITexture;
	class ILayout;
	class IProgram;
	class IBuffer;
	class IModel;
	class IMesh;
}

NS_SHIBBOLETH

using ShaderResourceViewPtr = Gaff::RefPtr<Gleam::IShaderResourceView>;
using ProgramBuffersPtr = Gaff::RefPtr<Gleam::IProgramBuffers>;
using RenderTargetPtr = Gaff::RefPtr<Gleam::IRenderTarget>;
using SamplerStatePtr = Gaff::RefPtr<Gleam::ISamplerState>;
using RenderStatePtr = Gaff::RefPtr<Gleam::IRenderState>;
//using CommandListPtr = Gaff::RefPtr<Gleam::ICommandList>;
using TexturePtr = Gaff::RefPtr<Gleam::ITexture>;
using LayoutPtr = Gaff::RefPtr<Gleam::ILayout>;
using ProgramPtr = Gaff::RefPtr<Gleam::IProgram>;
using ShaderPtr = Gaff::RefPtr<Gleam::IShader>;
using BufferPtr = Gaff::RefPtr<Gleam::IBuffer>;
using ModelPtr = Gaff::RefPtr<Gleam::IModel>;
using MeshPtr = Gaff::RefPtr<Gleam::IMesh>;

using MaterialMapping = Gaff::Pair<size_t, Gleam::IShader::SHADER_TYPE>;

template <class T>
struct SingleDataWrapper : public Gaff::IVirtualDestructor
{
	T data;
};

struct TextureData : public Gaff::IVirtualDestructor
{
	Array<ShaderResourceViewPtr> resource_views;
	Array<TexturePtr> textures;
	Gaff::Image image;
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
	ResourceWrapper<ShaderData> shaders[Gleam::IShader::SHADER_TYPE_SIZE];
	Array<ProgramPtr> programs;
	RenderModes render_mode;
};

struct RenderTargetData : public Gaff::IVirtualDestructor
{
	Array<RenderTargetPtr> render_targets;
	Array< Array<TexturePtr> > textures;
	Array<TexturePtr> depth_stencils;
	Array< Array<ShaderResourceViewPtr> > texture_srvs;
	Array<ShaderResourceViewPtr> depth_stencil_srvs;

	// Used for calculating aspect ratio.
	unsigned int width;
	unsigned int height;

	unsigned short tags;
	bool any_display_with_tags;
};

using ProgramBuffersData = SingleDataWrapper< Array<ProgramBuffersPtr> >;
using SamplerStateData = SingleDataWrapper< Array<SamplerStatePtr> >;
using BufferData = SingleDataWrapper< Array<BufferPtr> >;

NS_END
