/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include <Shibboleth_ResourceManager.h>
#include <Gleam_IShader.h>
#include <Gleam_ILayout.h>

NS_SHIBBOLETH

class ShaderResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Vector<Gleam::IRenderDevice*> getDevices(void) const;

	bool createShaderAndLayout(const Vector<Gleam::IRenderDevice*>& devices, const char* shader_source, Gleam::IShader::Type shader_type);
	bool createShaderAndLayout(Gleam::IRenderDevice& device, const char* shader_source, Gleam::IShader::Type shader_type);

	const Gleam::IShader* getShader(const Gleam::IRenderDevice& rd) const;
	Gleam::IShader* getShader(const Gleam::IRenderDevice& rd);

	const Gleam::ILayout* getLayout(const Gleam::IRenderDevice& rd) const;
	Gleam::ILayout* getLayout(const Gleam::IRenderDevice& rd);

private:
	using ShaderLayoutPair = eastl::pair< UniquePtr<Gleam::IShader>, UniquePtr<Gleam::ILayout> >;

	VectorMap<const Gleam::IRenderDevice*, ShaderLayoutPair> _shader_data{ ProxyAllocator("Graphics") };

	SHIB_REFLECTION_CLASS_DECLARE(ShaderResource);
};

using ShaderResourcePtr = Gaff::RefPtr<ShaderResource>;

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ShaderResource)
