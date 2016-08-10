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
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_IResourceManager.h>
#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_ISchemaManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IProgram.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

class ShaderLoadedFunctor : public ResourceLoaderFunctorBase
{
public:
	ShaderLoadedFunctor(
		ResourceContainer* res_cont, ProgramData* program_data,
		ShaderProgramLoader* loader
	):
		_program_data(program_data), _loader(loader),
		_res_cont(res_cont)
	{
	}

	~ShaderLoadedFunctor(void) {}

	void operator()(ResourceContainer* res_cont)
	{
		if (res_cont->isLoaded()) {
			ShaderData* shader_data = res_cont->getResourcePtr<ShaderData>();
			_program_data->shaders[shader_data->shader_type] = shader_data;

			// All the resources haven't loaded yet.
			if (!subResourceLoaded(_res_cont)) {
				return;
			}

			if (!_loader->createPrograms(_program_data)) {
				failedToLoadResource(_res_cont);
				return;
			}

			resourceLoadSuccessful(_res_cont);

		} else if (!_res_cont->hasFailed()) {
			failedToLoadResource(_res_cont);
		}
	}

private:
	ProgramData* _program_data;
	ShaderProgramLoader* _loader;
	ResourceContainer* _res_cont;
};

ShaderProgramLoader::ShaderProgramLoader(IResourceManager& res_mgr, ISchemaManager& schema_mgr, IRenderManager& render_mgr):
	_res_mgr(res_mgr), _schema_mgr(schema_mgr), _render_mgr(render_mgr)
{
}

ShaderProgramLoader::~ShaderProgramLoader(void)
{
}

ResourceLoadData ShaderProgramLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		return { nullptr };
	}

	if (!json.validate(_schema_mgr.getSchema("Material.schema"))) {
		return { nullptr };
	}

	Gaff::JSON vertex = json["Vertex"];
	Gaff::JSON pixel = json["Pixel"];
	Gaff::JSON hull = json["Hull"];
	Gaff::JSON geometry = json["Geometry"];
	Gaff::JSON domain = json["Domain"];
	Gaff::JSON render_pass = json["Render Pass"];

	GAFF_ASSERT(
		vertex.isString() || pixel.isString() || hull.isString() ||
		geometry.isString() || domain.isString()
	);

	GAFF_ASSERT(render_pass.isString());
	
	ProgramData* program_data = SHIB_ALLOCT(ProgramData, *GetAllocator());

	if (!program_data) {
		return { nullptr };
	}

	program_data->render_pass = GetEnumRefDef<RenderPasses>().getValue(render_pass.getString());

	if (!createRasterStates(program_data, json)) {
		SHIB_FREET(program_data, *GetAllocator());
		return { nullptr };
	}

	auto callback = Gaff::Bind<ShaderLoadedFunctor, void, ResourceContainer*>(ShaderLoadedFunctor(res_cont, program_data, this));
	ResourceLoadData res_load_data { program_data };
	res_load_data.sub_res_data.reserve(5);

	memset(program_data->shaders, 0, sizeof(program_data->shaders));

	const char* shader_ext = +_render_mgr.getShaderExtension();

	if (vertex.isString()) {
		SubResourceData sub_data = { AString(vertex.getString()) + shader_ext, Gleam::IShader::SHADER_VERTEX, callback };
		res_load_data.sub_res_data.emplacePush(sub_data);
	}

	if (pixel.isString()) {
		SubResourceData sub_data = { AString(pixel.getString()) + shader_ext, Gleam::IShader::SHADER_PIXEL, callback };
		res_load_data.sub_res_data.emplacePush(sub_data);
	}

	if (hull.isString()) {
		SubResourceData sub_data = { AString(hull.getString()) + shader_ext, Gleam::IShader::SHADER_HULL, callback };
		res_load_data.sub_res_data.emplacePush(sub_data);
	}

	if (geometry.isString()) {
		SubResourceData sub_data = { AString(geometry.getString()) + shader_ext, Gleam::IShader::SHADER_GEOMETRY, callback };
		res_load_data.sub_res_data.emplacePush(sub_data);
	}

	if (domain.isString()) {
		SubResourceData sub_data = { AString(domain.getString()) + shader_ext, Gleam::IShader::SHADER_DOMAIN, callback };
		res_load_data.sub_res_data.emplacePush(sub_data);
	}

	return res_load_data;
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
			ShaderData* vert_shaders = data->shaders[Gleam::IShader::SHADER_VERTEX];
			program->attach(vert_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_PIXEL]) {
			ShaderData* pixel_shaders = data->shaders[Gleam::IShader::SHADER_PIXEL];
			program->attach(pixel_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_HULL]) {
			ShaderData* hull_shaders = data->shaders[Gleam::IShader::SHADER_HULL];
			program->attach(hull_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_GEOMETRY]) {
			ShaderData* geo_shaders = data->shaders[Gleam::IShader::SHADER_GEOMETRY];
			program->attach(geo_shaders->shaders[i].get());
		}

		if (data->shaders[Gleam::IShader::SHADER_DOMAIN]) {
			ShaderData* domain_shaders = data->shaders[Gleam::IShader::SHADER_DOMAIN];
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
	settings.slope_scale_depth_bias = static_cast<float>(ss_depth_bias.getDouble(0.0f));
	settings.depth_bias_clamp = static_cast<float>(depth_bias_clamp.getDouble(0.0f));
	settings.depth_bias = static_cast<int>(depth_bias.getInt(0));
	settings.front_face_counter_clockwise = ffcc.isTrue();
	settings.scissor_enabled = scissor.isTrue();
	settings.two_sided = two_sided.isTrue();
	settings.wireframe = wireframe.isTrue();

	data->raster_states = _render_mgr.getOrCreateRasterStates(Gaff::FNV1aHash32T(&settings), settings);
	return true;
}

NS_END
