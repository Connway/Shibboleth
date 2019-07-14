/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_GraphicsReflection.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(ShaderResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ShaderResource)
	.classAttrs(
		//CreatableAttribute(),
		ResExtAttribute(".shader.bin"),
		ResExtAttribute(".shader"),
		MakeLoadFileCallbackAttribute(&ShaderResource::loadShader)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ShaderResource)

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

void ShaderResource::loadShader(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load shader '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	Gleam::IShader::ShaderType shader_type = Gleam::IShader::SHADER_TYPE_SIZE;
	const Vector<Gleam::IRenderDevice*>* devices = nullptr;
	IFile* shader_file = nullptr;
	U8String shader_file_path;

	GAFF_REF(shader_type);

	{
		const auto guard = reader.enterElementGuard("devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed shader '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char* const tag = reader.readString("main");
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load shader '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer());
		failed();
		return;
	}

	{
		const auto guard = reader.enterElementGuard("shader_type");

		if (!reader.isString()) {
			LogErrorResource("Malformed shader '%s'. 'shader_type' element is not a string.", getFilePath().getBuffer());
			failed();
			return;
		}

		Reflection<Gleam::IShader::ShaderType>::Load(reader, shader_type);
	}

	// Should we put out a read job instead of loading here?
	// Or just assume nowadays most people have SSDs or decent speed hard drives?
	{
		const auto guard = reader.enterElementGuard("shader_file");

		if (!reader.isString()) {
			LogErrorResource("Malformed shader '%s'. 'shader_file' element is not a string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char* const path = reader.readString();

		shader_file_path = path;
		shader_file = res_mgr.loadFileAndWait(path);

		reader.freeString(path);
	}

	if (!shader_file) {
		LogErrorResource("Failed to load shader '%s'. Unable to find or load file '%s'.", getFilePath().getBuffer(), shader_file_path.data());
		failed();
		return;
	}

	for (Gleam::IRenderDevice* rd : *devices) {
		Gleam::IShader* const shader = render_mgr.createShader();

		if (!shader->initSource(*rd, reinterpret_cast<const char*>(shader_file->getBuffer()), shader_file->size(), shader_type)) {
			LogErrorResource("Failed to load shader '%s'. Shader compilation failed.", getFilePath().getBuffer());
			failed();
		}

		auto& sd = _shader_data[rd];
		sd.first.reset(shader);
		//sd.second.reset(layout);
	}
}

NS_END
