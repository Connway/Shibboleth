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

#include "Shibboleth_SamplerStateLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_ISamplerState.h>
#include <Gleam_IRenderDevice.h>

NS_SHIBBOLETH

SamplerStateLoader::SamplerStateLoader(IRenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

SamplerStateLoader::~SamplerStateLoader(void)
{
}

ResourceLoadData SamplerStateLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	const char* file_name = res_cont->getResourceKey().getBuffer();
	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to parse file '%s'.\n", file_name);
		return { nullptr };
	}

	if (!json.isObject()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Sampler State file '%s' is malformed.\n", file_name);
		return { nullptr };
	}

	if (!json["filter"].isString()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Sampler State file '%s' is malformed. Value at 'filter' is not a string.\n", file_name);
		return { nullptr };
	}

	if (!json["wrap_u"].isString() || !json["wrap_v"].isString() || !json["wrap_w"].isString()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Sampler State file '%s' is malformed. Value at 'wrap_u/v/w' is not a string.\n", file_name);
		return { nullptr };
	}

	if (!json["min_lod"].isDouble() || !json["max_lod"].isDouble() || !json["lod_bias"].isDouble()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Sampler State file '%s' is malformed. Value at 'min/max_lod' and/or 'lod_bias' is not a float.\n", file_name);
		return { nullptr };
	}

	Gleam::ISamplerState::SamplerSettings ss = {
		GetEnumRefDef<Gleam::ISamplerState::FILTER>().getValue(json["filter"].getString()),
		GetEnumRefDef<Gleam::ISamplerState::WRAP>().getValue(json["wrap_u"].getString()),
		GetEnumRefDef<Gleam::ISamplerState::WRAP>().getValue(json["wrap_v"].getString()),
		GetEnumRefDef<Gleam::ISamplerState::WRAP>().getValue(json["wrap_w"].getString()),
		static_cast<float>(json["min_lod"].getDouble()),
		static_cast<float>(json["max_lod"].getDouble()),
		static_cast<float>(json["lod_bias"].getDouble()),
		0,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	if (ss.filter == Gleam::ISamplerState::FILTER_ANISOTROPIC && !json["max_anisotropy"].isInt()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Sampler State file '%s' is malformed. Value at 'max_anisotropy' is not an integer.\n", file_name);
		return { nullptr };
	}

	if ((ss.wrap_u == Gleam::ISamplerState::WRAP_BORDER || ss.wrap_v == Gleam::ISamplerState::WRAP_BORDER ||
		ss.wrap_w == Gleam::ISamplerState::WRAP_BORDER) && (!json["border_r"].isDouble() || !json["border_g"].isDouble() ||
		!json["border_b"].isDouble() || !json["border_a"].isDouble())) {

		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Sampler State file '%s' is malformed. A wrap mode is defined as 'BORDER' and value 'border_r/g/b/a' is not a float.\n", file_name);
		return { nullptr };
	}

	if (ss.filter == Gleam::ISamplerState::FILTER_ANISOTROPIC) {
		ss.max_anisotropy = static_cast<unsigned int>(json["max_anisotropy"].getInt());
	}

	if (ss.wrap_u == Gleam::ISamplerState::WRAP_BORDER || ss.wrap_v == Gleam::ISamplerState::WRAP_BORDER ||
		ss.wrap_w == Gleam::ISamplerState::WRAP_BORDER) {

		ss.border_r = static_cast<float>(json["border_r"].getDouble());
		ss.border_g = static_cast<float>(json["border_g"].getDouble());
		ss.border_b = static_cast<float>(json["border_b"].getDouble());
		ss.border_a = static_cast<float>(json["border_a"].getDouble());
	}

	SamplerStateData* sampler_data = SHIB_ALLOCT(SamplerStateData, *GetAllocator());

	if (!sampler_data) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate Sampler State data structure.\n", file_name);
		return { nullptr };
	}

	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	
	sampler_data->data.resize(rd.getNumDevices());

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		SamplerStatePtr sampler(_render_mgr.createSamplerState());
		rd.setCurrentDevice(i);

		if (!sampler) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate Sampler State for sampler '%s'.\n", file_name);
			SHIB_FREET(sampler_data, *GetAllocator());
			return { nullptr };
		}

		if (!sampler->init(rd, ss)) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize Sampler State for sampler '%s'.\n", file_name);
			SHIB_FREET(sampler_data, *GetAllocator());
			return { nullptr };
		}

		sampler_data->data[i] = std::move(sampler);
	}

	return { sampler_data };
}

NS_END