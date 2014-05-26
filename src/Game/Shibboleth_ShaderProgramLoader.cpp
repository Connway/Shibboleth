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

#include "Shibboleth_ShaderProgramLoader.h"
#include "Shibboleth_RenderManager.h"
#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gaff_File.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

ShaderProgramLoader::ShaderProgramLoader(RenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

ShaderProgramLoader::~ShaderProgramLoader(void)
{
}

Gaff::IVirtualDestructor* ShaderProgramLoader::load(const char* file_name, unsigned long long)
{
	Gaff::JSON json;

	if (!json.parseFile(file_name)) {
		return false;
	}

	Gaff::JSON vertex = json["vertex"];
	Gaff::JSON pixel = json["pixel"];
	Gaff::JSON hull = json["hull"];
	Gaff::JSON geometry = json["geometry"];
	Gaff::JSON domain = json["domain"];

	assert(!vertex.isNull() || !pixel.isNull() || !hull.isNull()
			|| !geometry.isNull() || !domain.isNull());

	Array<ShaderPtr> vertex_shader, pixel_shader, hull_shader;
	Array<ShaderPtr> geometry_shader, domain_shader;

	if (vertex.isString()) {
		vertex_shader = loadShaders(vertex.getString(), Gleam::IShader::SHADER_VERTEX);
	}

	if (pixel.isString()) {
		pixel_shader = loadShaders(pixel.getString(), Gleam::IShader::SHADER_PIXEL);
	}

	if (hull.isString()) {
		hull_shader = loadShaders(hull.getString(), Gleam::IShader::SHADER_HULL);
	}

	if (geometry.isString()) {
		geometry_shader = loadShaders(geometry.getString(), Gleam::IShader::SHADER_GEOMETRY);
	}

	if (domain.isString()) {
		domain_shader = loadShaders(domain.getString(), Gleam::IShader::SHADER_DOMAIN);
	}

	// If none of our shaders compiled, you dun-goofed.
	if (vertex_shader.empty() && pixel_shader.empty() && hull_shader.empty() && geometry_shader.empty() && domain_shader.empty()) {
		// log error
		return nullptr;
	}

	Array<ProgramPtr> programs = createPrograms(vertex_shader, pixel_shader, hull_shader, geometry_shader, domain_shader);

	ProgramData* program_data = GetAllocator().template allocT<ProgramData>();

	if (!program_data) {
		return nullptr;
	}

	program_data->programs = programs;

	return program_data;
}

Array<ShaderPtr> ShaderProgramLoader::loadShaders(const char* file_name, Gleam::IShader::SHADER_TYPE shader_type)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr.getSpinLock());
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	Array<ShaderPtr> shaders;

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		ShaderPtr shader(_render_mgr.createShader());
		rd.setCurrentDevice(i);

		if (!shader) {
			shaders.clear();
			break;
		}

		if (!shader->init(rd, file_name, shader_type)) {
			shaders.clear();
			break;
		}

		shaders.push(shader);
	}

	return shaders;
}

Array<ProgramPtr> ShaderProgramLoader::createPrograms(
	Array<ShaderPtr>& vert_shaders, Array<ShaderPtr>& pixel_shaders, Array<ShaderPtr>& hull_shaders,
	Array<ShaderPtr>& geometry_shaders, Array<ShaderPtr>& domain_shaders)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr.getSpinLock());
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	Array<ProgramPtr> programs;

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		ProgramPtr program(_render_mgr.createProgram());
		rd.setCurrentDevice(i);

		if (!program) {
			programs.clear();
			break;
		}

		if (!program->init()) {
			programs.clear();
			break;
		}

		if (!vert_shaders.empty()) {
			program->attach(vert_shaders[i].get());
		}

		if (!pixel_shaders.empty()) {
			program->attach(pixel_shaders[i].get());
		}

		if (!hull_shaders.empty()) {
			program->attach(hull_shaders[i].get());
		}

		if (!geometry_shaders.empty()) {
			program->attach(geometry_shaders[i].get());
		}

		if (!domain_shaders.empty()) {
			program->attach(domain_shaders[i].get());
		}

		programs.push(program);
	}

	return programs;
}

NS_END
