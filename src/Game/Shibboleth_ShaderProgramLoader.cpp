/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_SchemaManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IProgram.h>
#include <Gaff_ScopedExit.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

ShaderProgramLoader::ShaderProgramLoader(ResourceManager& res_mgr, SchemaManager& schema_mgr, RenderManager& render_mgr):
	_res_mgr(res_mgr), _schema_mgr(schema_mgr), _render_mgr(render_mgr)
{
}

ShaderProgramLoader::~ShaderProgramLoader(void)
{
}

Gaff::IVirtualDestructor* ShaderProgramLoader::load(const char* file_name, uint64_t, HashMap<AString, IFile*>& file_map)
{
	assert(file_map.hasElementWithKey(AString(file_name)));

	GAFF_SCOPE_EXIT([&]()
	{
		for (auto it = file_map.begin(); it != file_map.end(); ++it) {
			if (*it) {
				GetApp().getFileSystem()->closeFile(*it);
			}
		}
	});

	IFile* file = file_map[AString(file_name)];

	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		return nullptr;
	}

	if (!json.validate(_schema_mgr.getSchema("Material.schema"))) {
		return nullptr;
	}

	Gaff::JSON vertex = json["Vertex"];
	Gaff::JSON pixel = json["Pixel"];
	Gaff::JSON hull = json["Hull"];
	Gaff::JSON geometry = json["Geometry"];
	Gaff::JSON domain = json["Domain"];
	Gaff::JSON render_pass = json["Render Pass"];

	assert(vertex.isString() || pixel.isString() || hull.isString() ||
			geometry.isString() || domain.isString());

	assert(render_pass.isString());
	
	ProgramData* program_data = GetAllocator()->template allocT<ProgramData>();

	if (!program_data) {
		return nullptr;
	}

	program_data->render_pass = GetEnumRefDef<RenderPasses>().getValue(render_pass.getString());

	if (vertex.isString() && !loadShader(program_data, vertex.getString(), Gleam::IShader::SHADER_VERTEX, file_map)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (pixel.isString() && !loadShader(program_data, pixel.getString(), Gleam::IShader::SHADER_PIXEL, file_map)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (hull.isString() && !loadShader(program_data, hull.getString(), Gleam::IShader::SHADER_HULL, file_map)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (geometry.isString() && !loadShader(program_data, geometry.getString(), Gleam::IShader::SHADER_GEOMETRY, file_map)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (domain.isString() && !loadShader(program_data, domain.getString(), Gleam::IShader::SHADER_DOMAIN, file_map)) {
		// log error
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (!createPrograms(program_data)) {
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	if (!createRasterStates(program_data, json)) {
		GetAllocator()->freeT(program_data);
		return nullptr;
	}

	return program_data;
}

bool ShaderProgramLoader::loadShader(ProgramData* data, const char* file_name, Gleam::IShader::SHADER_TYPE shader_type, HashMap<AString, IFile*>& file_map)
{
	AString fname = AString(file_name) + _render_mgr.getShaderExtension();
	data->shaders[shader_type] = _res_mgr.loadResourceImmediately(fname.getBuffer(), shader_type, file_map);

	// Loop until loaded
	while (!data->shaders[shader_type].getResourcePtr()->isLoaded() &&
			!data->shaders[shader_type].getResourcePtr()->hasFailed()) {
	}

	return !data->shaders[shader_type].getResourcePtr()->hasFailed();
}

bool ShaderProgramLoader::createPrograms(ProgramData* data)
{
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	data->programs.reserve(rd.getNumDevices());

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

		data->programs.emplacePush(program);
	}

	return true;
}

bool ShaderProgramLoader::createRasterStates(ProgramData* data, const Gaff::JSON& json)
{
	Gaff::JSON ss_depth_bias = json["Slope Scale Depth Bias"];
	Gaff::JSON depth_bias_clamp = json["Depth Bias Clamp"];
	Gaff::JSON depth_bias = json["Depth Bias"];
	Gaff::JSON ffcc = json["Front Face Counter Clockwise"];
	Gaff::JSON scissor = json["Scissor Enabled"];
	Gaff::JSON two_sided = json["Two Sided"];
	Gaff::JSON wireframe = json["Wireframe"];

	Gleam::IRasterState::RasterStateSettings settings;
	settings.slope_scale_depth_bias = static_cast<float>(ss_depth_bias.getReal(0.0f));
	settings.depth_bias_clamp = static_cast<float>(depth_bias_clamp.getReal(0.0f));
	settings.depth_bias = static_cast<int>(depth_bias.getInteger(0));
	settings.front_face_counter_clockwise = ffcc.isTrue();
	settings.scissor_enabled = scissor.isTrue();
	settings.two_sided = two_sided.isTrue();
	settings.wireframe = wireframe.isTrue();

	data->raster_states = _render_mgr.getOrCreateRasterStates(Gaff::FNV1aHash32T(&settings), settings);
	return true;
}

NS_END
