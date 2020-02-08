/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_ShaderResource.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE_BEGIN_NEW(ShaderResource)
	.classAttrs(
		CreatableAttribute()
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END_NEW(ShaderResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_NEW(ShaderResource)

Vector<Gleam::IRenderDevice*> ShaderResource::getDevices(void) const
{
	Vector<Gleam::IRenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _shader_data) {
		out.emplace_back(const_cast<Gleam::IRenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool ShaderResource::createShaderAndLayout(const Vector<Gleam::IRenderDevice*>& devices, const char* shader_source, Gleam::IShader::ShaderType shader_type)
{
	bool success = true;

	for (Gleam::IRenderDevice* device : devices) {
		success = success && createShaderAndLayout(*device, shader_source, shader_type);
	}

	return success;
}

bool ShaderResource::createShaderAndLayout(Gleam::IRenderDevice& device, const char* shader_source, Gleam::IShader::ShaderType shader_type)
{
	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	Gleam::IShader* const shader = render_mgr.createShader();

	if (!shader->initSource(device, shader_source, shader_type)) {
		LogErrorResource("Failed to create shader '%s'.", getFilePath().getBuffer());
		SHIB_FREET(shader, GetAllocator());
		return false;
	}

	ShaderLayoutPair& pair = _shader_data[&device];
	pair.first.reset(shader);

	if (shader_type == Gleam::IShader::SHADER_VERTEX) {
		Gleam::ILayout* const layout = render_mgr.createLayout();

		if (!layout->init(device, *shader)) {
			LogErrorResource("Failed to create shader layout '%s'.", getFilePath().getBuffer());
			SHIB_FREET(shader, GetAllocator());
			SHIB_FREET(layout, GetAllocator());
			return false;
		}

		pair.second.reset(layout);
	}

	return true;
}

const Gleam::IShader* ShaderResource::getShader(const Gleam::IRenderDevice& rd) const
{
	const auto it = _shader_data.find(&rd);
	return (it != _shader_data.end()) ? it->second.first.get() : nullptr;
}

Gleam::IShader* ShaderResource::getShader(const Gleam::IRenderDevice& rd)
{
	const auto it = _shader_data.find(&rd);
	return (it != _shader_data.end()) ? it->second.first.get() : nullptr;
}

const Gleam::ILayout* ShaderResource::getLayout(const Gleam::IRenderDevice& rd) const
{
	const auto it = _shader_data.find(&rd);
	return (it != _shader_data.end()) ? it->second.second.get() : nullptr;
}

Gleam::ILayout* ShaderResource::getLayout(const Gleam::IRenderDevice& rd)
{
	const auto it = _shader_data.find(&rd);
	return (it != _shader_data.end()) ? it->second.second.get() : nullptr;
}

NS_END
