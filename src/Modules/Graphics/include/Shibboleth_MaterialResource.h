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

#include "Shibboleth_ShaderResource.h"
#include <Shibboleth_ResourcePtr.h>
#include <Shibboleth_SmartPtrs.h>
#include <Gleam_IProgram.h>

NS_SHIBBOLETH

class MaterialResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	void load(const ISerializeReader& reader, uintptr_t thread_id_int) override;

	Vector<Gleam::IRenderDevice*> getDevices(void) const;

	bool createProgram(
		const Vector<Gleam::IRenderDevice*>& devices,
		ResourcePtr<ShaderResource>& vertex,
		ResourcePtr<ShaderResource>& pixel,
		ResourcePtr<ShaderResource>& domain,
		ResourcePtr<ShaderResource>& geometry,
		ResourcePtr<ShaderResource>& hull
	);

	bool createProgram(
		Gleam::IRenderDevice& device,
		ResourcePtr<ShaderResource>& vertex,
		ResourcePtr<ShaderResource>& pixel,
		ResourcePtr<ShaderResource>& domain,
		ResourcePtr<ShaderResource>& geometry,
		ResourcePtr<ShaderResource>& hull
	);

	bool createProgram(const Vector<Gleam::IRenderDevice*>& devices, ResourcePtr<ShaderResource>& vertex, ResourcePtr<ShaderResource>& pixel);
	bool createProgram(Gleam::IRenderDevice& device, ResourcePtr<ShaderResource>& vertex, ResourcePtr<ShaderResource>& pixel);

	bool createProgram(const Vector<Gleam::IRenderDevice*>& devices, ResourcePtr<ShaderResource>& compute);
	bool createProgram(Gleam::IRenderDevice& device, ResourcePtr<ShaderResource>& compute);

	const Gleam::IProgram* getProgram(const Gleam::IRenderDevice& device) const;
	Gleam::IProgram* getProgram(const Gleam::IRenderDevice& device);

	const Gleam::ILayout* getLayout(const Gleam::IRenderDevice& device) const;
	Gleam::ILayout* getLayout(const Gleam::IRenderDevice& device);

private:
	VectorMap< const Gleam::IRenderDevice*, UniquePtr<Gleam::IProgram> > _programs{ ProxyAllocator("Graphics") };
	ResourcePtr<ShaderResource> _shaders[static_cast<size_t>(Gleam::IShader::Type::Count)];

	SHIB_REFLECTION_CLASS_DECLARE(MaterialResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::MaterialResource)
