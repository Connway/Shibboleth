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

#include "Shibboleth_GraphicsDefines.h"
#include <Shibboleth_ResourceManager.h>
#include <Gleam_Shader.h>
#include <Gleam_Layout.h>

NS_GLEAM
	class RenderDevice;
NS_END

NS_SHIBBOLETH

class ShaderResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Vector<Gleam::RenderDevice*> getDevices(void) const;

	bool createShaderAndLayout(const Vector<Gleam::RenderDevice*>& devices, const char* shader_source, Gleam::IShader::Type shader_type);
	bool createShaderAndLayout(Gleam::RenderDevice& device, const char* shader_source, Gleam::IShader::Type shader_type);

	const Gleam::Shader* getShader(const Gleam::RenderDevice& rd) const;
	Gleam::Shader* getShader(const Gleam::RenderDevice& rd);

	const Gleam::Layout* getLayout(const Gleam::RenderDevice& rd) const;
	Gleam::Layout* getLayout(const Gleam::RenderDevice& rd);

private:
	using ShaderLayoutPair = eastl::pair< UniquePtr<Gleam::Shader>, UniquePtr<Gleam::Layout> >;

	VectorMap<const Gleam::RenderDevice*, ShaderLayoutPair> _shader_data{ GRAPHICS_ALLOCATOR };

	SHIB_REFLECTION_CLASS_DECLARE(ShaderResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ShaderResource)
