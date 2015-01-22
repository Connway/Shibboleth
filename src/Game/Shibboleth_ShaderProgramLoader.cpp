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

#include "Shibboleth_ShaderProgramLoader.h"
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gaff_ScopedLock.h>
#include <Gaff_File.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

ShaderProgramLoader::ShaderProgramLoader(ResourceManager& res_mgr, RenderManager& render_mgr, IFileSystem& file_system):
	_res_mgr(res_mgr), _render_mgr(render_mgr), _file_system(file_system)
{
}

ShaderProgramLoader::~ShaderProgramLoader(void)
{
}

Gaff::IVirtualDestructor* ShaderProgramLoader::load(const char* file_name, unsigned long long)
{
	ProgramData* program_data = GetAllocator()->template allocT<ProgramData>();

	if (!program_data) {
		return nullptr;
	}

	IFile* file = _file_system.openFile(file_name);

	if (!file) {
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		GetAllocator()->freeT(program_data);
		_file_system.closeFile(file);
		return nullptr;
	}

	_file_system.closeFile(file);

	Gaff::JSON vertex = json["vertex"];
	Gaff::JSON pixel = json["pixel"];
	Gaff::JSON hull = json["hull"];
	Gaff::JSON geometry = json["geometry"];
	Gaff::JSON domain = json["domain"];

	assert(!vertex.isNull() || !pixel.isNull() || !hull.isNull() ||
			!geometry.isNull() || !domain.isNull());

	if (vertex.isString() && !loadShader(program_data, vertex.getString(), Gleam::IShader::SHADER_VERTEX)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (pixel.isString() && !loadShader(program_data, pixel.getString(), Gleam::IShader::SHADER_PIXEL)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (hull.isString() && !loadShader(program_data, hull.getString(), Gleam::IShader::SHADER_HULL)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (geometry.isString() && !loadShader(program_data, geometry.getString(), Gleam::IShader::SHADER_GEOMETRY)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (domain.isString() && !loadShader(program_data, domain.getString(), Gleam::IShader::SHADER_DOMAIN)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (!createPrograms(program_data)) {
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	return program_data;
}


bool ShaderProgramLoader::loadShader(ProgramData* data, const char* file_name, Gleam::IShader::SHADER_TYPE shader_type)
{
	AString fname = AString(file_name) + _render_mgr.getShaderExtension();
	data->shaders[shader_type] = _res_mgr.loadResourceImmediately(fname.getBuffer(), shader_type);

	// Loop until loaded
	while (!data->shaders[shader_type].getResourcePtr()->isLoaded() &&
			!data->shaders[shader_type].getResourcePtr()->hasFailed()) {
	}

	return !data->shaders[shader_type].getResourcePtr()->hasFailed();
}

bool ShaderProgramLoader::createPrograms(ProgramData* data)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr.getSpinLock());
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		ProgramPtr program(_render_mgr.createProgram());
		rd.setCurrentDevice(i);

		if (!program) {
			data->programs.clear();
			return false;
		}

		if (!program->init()) {
			data->programs.clear();
			return false;
		}

		if (data->shaders[Gleam::IShader::SHADER_VERTEX]) {
			ShaderData* vert_shaders = data->shaders[Gleam::IShader::SHADER_VERTEX].getDataPtr();
			program->attach(vert_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_PIXEL]) {
			ShaderData* pixel_shaders = data->shaders[Gleam::IShader::SHADER_PIXEL].getDataPtr();
			program->attach(pixel_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_HULL]) {
			ShaderData* hull_shaders = data->shaders[Gleam::IShader::SHADER_HULL].getDataPtr();
			program->attach(hull_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_GEOMETRY]) {
			ShaderData* geo_shaders = data->shaders[Gleam::IShader::SHADER_GEOMETRY].getDataPtr();
			program->attach(geo_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_DOMAIN]) {
			ShaderData* domain_shaders = data->shaders[Gleam::IShader::SHADER_DOMAIN].getDataPtr();
			program->attach(domain_shaders->shaders[i].get());
		}

		data->programs.push(program);
	}

	return true;
}

NS_END
