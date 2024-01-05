/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#ifdef GLEAM_USE_D3D11

#include "Gleam_Program_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Shader_Direct3D11.h"

namespace
{
	template <Gleam::IShader::Type ShaderType>
	struct ShaderBinder;

	template <>
	struct ShaderBinder<Gleam::IShader::Type::Vertex> final
	{
		static constexpr auto BindFunc = &ID3D11DeviceContext::VSSetShader;
		using ShaderType = ID3D11VertexShader;
	};

	template <>
	struct ShaderBinder<Gleam::IShader::Type::Pixel> final
	{
		static constexpr auto BindFunc = &ID3D11DeviceContext::PSSetShader;
		using ShaderType = ID3D11PixelShader;
	};

	template <>
	struct ShaderBinder<Gleam::IShader::Type::Domain> final
	{
		static constexpr auto BindFunc = &ID3D11DeviceContext::DSSetShader;
		using ShaderType = ID3D11DomainShader;
	};

	template <>
	struct ShaderBinder<Gleam::IShader::Type::Geometry> final
	{
		static constexpr auto BindFunc = &ID3D11DeviceContext::GSSetShader;
		using ShaderType = ID3D11GeometryShader;
	};

	template <>
	struct ShaderBinder<Gleam::IShader::Type::Hull> final
	{
		static constexpr auto BindFunc = &ID3D11DeviceContext::HSSetShader;
		using ShaderType = ID3D11HullShader;
	};

	template <>
	struct ShaderBinder<Gleam::IShader::Type::Compute> final
	{
		static constexpr auto BindFunc = &ID3D11DeviceContext::CSSetShader;
		using ShaderType = ID3D11ComputeShader;
	};


	template <Gleam::IShader::Type ShaderType>
	static void BindShader(
		ID3D11DeviceContext3* context,
		ID3D11DeviceChild* shader, 
		ID3D11ClassInstance* const* class_instances = NULL,
		UINT num_class_instances = 0)
	{
		(context->*ShaderBinder<ShaderType>::BindFunc)(
			static_cast<ShaderBinder<ShaderType>::ShaderType*>(shader),
			class_instances,
			num_class_instances
		);
	}

	using ShaderBindFunc = void (*)(ID3D11DeviceContext3*, ID3D11DeviceChild*, ID3D11ClassInstance* const*, UINT);
	static constexpr ShaderBindFunc k_bind_shader_funcs[static_cast<size_t>(Gleam::IShader::Type::Count)] =
	{
		&BindShader<Gleam::IShader::Type::Vertex>,
		&BindShader<Gleam::IShader::Type::Pixel>,
		&BindShader<Gleam::IShader::Type::Domain>,
		&BindShader<Gleam::IShader::Type::Geometry>,
		&BindShader<Gleam::IShader::Type::Hull>,
		&BindShader<Gleam::IShader::Type::Compute>
	};
}



NS_GLEAM

void Program::attach(IShader* shader)
{
	GAFF_ASSERT(shader->getRendererType() == RendererType::Direct3D11);

	const IShader::Type shader_type = shader->getType();
	GAFF_ASSERT(static_cast<int32_t>(shader->getType()) >= 0 && shader_type < IShader::Type::Count);

	_attached_shaders[static_cast<int32_t>(shader_type)] = static_cast<Shader*>(shader);

	GAFF_COM_SAFE_RELEASE(_d3d_shaders[static_cast<int32_t>(shader_type)]);
	_d3d_shaders[static_cast<int32_t>(shader_type)] = static_cast<Shader*>(shader)->getShader();
}

void Program::detach(IShader::Type shader)
{
	GAFF_ASSERT(static_cast<int32_t>(shader) >= 0 && shader < IShader::Type::Count);
	_attached_shaders[static_cast<size_t>(shader)] = nullptr;

	GAFF_COM_SAFE_RELEASE(_d3d_shaders[static_cast<int32_t>(shader)]);
	_d3d_shaders[static_cast<int32_t>(shader)] = nullptr;
}

void Program::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	for (const IShader::Type shader_type : Gaff::EnumIterator<IShader::Type>()) {
		k_bind_shader_funcs[static_cast<size_t>(shader_type)](
			context,
			_d3d_shaders[static_cast<size_t>(shader_type)],
			NULL,
			0
		);
	}
}

void Program::unbind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	for (const ShaderBindFunc bind_func : k_bind_shader_funcs) {
		bind_func(context, NULL, NULL, 0);
	}
}

const IShader* Program::getAttachedShader(IShader::Type type) const
{
	GAFF_ASSERT((static_cast<int32_t>(type) >= static_cast<int32_t>(IShader::Type::Vertex)) && (static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count));
	return _attached_shaders[static_cast<int32_t>(type)];
}

IShader* Program::getAttachedShader(IShader::Type type)
{
	GAFF_ASSERT((static_cast<int32_t>(type) >= static_cast<int32_t>(IShader::Type::Vertex)) && (static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count));
	return _attached_shaders[static_cast<int32_t>(type)];
}

RendererType Program::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
