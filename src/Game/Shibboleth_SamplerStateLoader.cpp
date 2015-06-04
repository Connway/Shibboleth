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

#include "Shibboleth_SamplerStateLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_TaskPoolTags.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_ISamplerState.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_ScopedExit.h>

NS_SHIBBOLETH

static EnumReflectionDefinition<Gleam::ISamplerState::FILTER> g_filter_ref_def =
Gaff::EnumRefDef<Gleam::ISamplerState::FILTER>("Gleam::ISamplerState::FILTER", ProxyAllocator("Reflection")).macroFix()
.addValue("NEAREST NEAREST NEAREST", Gleam::ISamplerState::FILTER_NEAREST_NEAREST_NEAREST)
.addValue("NEAREST LINEAR NEAREST", Gleam::ISamplerState::FILTER_NEAREST_LINEAR_NEAREST)
.addValue("NEAREST LINEAR Linear", Gleam::ISamplerState::FILTER_NEAREST_LINEAR_LINEAR)
.addValue("LINEAR NEAREST NEAREST", Gleam::ISamplerState::FILTER_LINEAR_NEAREST_NEAREST)
.addValue("LINEAR NEAREST Linear", Gleam::ISamplerState::FILTER_LINEAR_NEAREST_LINEAR)
.addValue("LINEAR LINEAR NEAREST", Gleam::ISamplerState::FILTER_LINEAR_LINEAR_NEAREST)
.addValue("LINEAR LINEAR LINEAR", Gleam::ISamplerState::FILTER_LINEAR_LINEAR_LINEAR)
.addValue("ANISOTROPIC", Gleam::ISamplerState::FILTER_ANISOTROPIC)
;

static EnumReflectionDefinition<Gleam::ISamplerState::WRAP> g_wrap_ref_def =
Gaff::EnumRefDef<Gleam::ISamplerState::WRAP>("Gleam::ISamplerState::FILTER", ProxyAllocator("Reflection")).macroFix()
.addValue("REPEAT", Gleam::ISamplerState::WRAP_REPEAT)
.addValue("MIRROR", Gleam::ISamplerState::WRAP_MIRROR)
.addValue("CLAMP", Gleam::ISamplerState::WRAP_CLAMP)
.addValue("BORDER", Gleam::ISamplerState::WRAP_BORDER)
;

SamplerStateLoader::SamplerStateLoader(RenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

SamplerStateLoader::~SamplerStateLoader(void)
{
}

Gaff::IVirtualDestructor* SamplerStateLoader::load(const char* file_name, unsigned long long, HashMap<AString, IFile*>& file_map)
{
	assert(file_map.hasElementWithKey(AString(file_name)));

	IFile* file = file_map[AString(file_name)];

	GAFF_SCOPE_EXIT([&]()
	{
		GetApp().getFileSystem()->closeFile(file);
		file_map[AString(file_name)] = nullptr;
	});

	Gaff::JSON json;
	
	if (!json.parse(file->getBuffer())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to parse file '%s'.\n", file_name);
		return nullptr;
	}

	if (!json.isObject()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Sampler State file '%s' is malformed.\n", file_name);
		return nullptr;
	}

	if (!json["filter"].isString()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Sampler State file '%s' is malformed. Value at 'filter' is not a string.\n", file_name);
		return nullptr;
	}

	if (!json["wrap_u"].isString() || !json["wrap_v"].isString() || !json["wrap_w"].isString()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Sampler State file '%s' is malformed. Value at 'wrap_u/v/w' is not a string.\n", file_name);
		return nullptr;
	}

	if (!json["min_lod"].isReal() || !json["max_lod"].isReal() || !json["lod_bias"].isReal()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Sampler State file '%s' is malformed. Value at 'min/max_lod' and/or 'lod_bias' is not a float.\n", file_name);
		return nullptr;
	}

	Gleam::ISamplerState::SamplerSettings ss = {
		g_filter_ref_def.getValue(json["filter"].getString()),
		g_wrap_ref_def.getValue(json["wrap_u"].getString()),
		g_wrap_ref_def.getValue(json["wrap_v"].getString()),
		g_wrap_ref_def.getValue(json["wrap_w"].getString()),
		static_cast<float>(json["min_lod"].getReal()),
		static_cast<float>(json["max_lod"].getReal()),
		static_cast<float>(json["lod_bias"].getReal()),
		0,
		0.0f, 0.0f, 0.0f, 0.0f
	};

	if (ss.filter == Gleam::ISamplerState::FILTER_ANISOTROPIC && !json["max_anisotropy"].isInteger()) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Sampler State file '%s' is malformed. Value at 'max_anisotropy' is not an integer.\n", file_name);
		return nullptr;
	}

	if ((ss.wrap_u == Gleam::ISamplerState::WRAP_BORDER || ss.wrap_v == Gleam::ISamplerState::WRAP_BORDER ||
		ss.wrap_w == Gleam::ISamplerState::WRAP_BORDER) && (!json["border_r"].isReal() || !json["border_g"].isReal() ||
		!json["border_b"].isReal() || !json["border_a"].isReal())) {

		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Sampler State file '%s' is malformed. A wrap mode is defined as 'BORDER' and value 'border_r/g/b/a' is not a float.\n", file_name);
		return nullptr;
	}

	if (ss.filter == Gleam::ISamplerState::FILTER_ANISOTROPIC) {
		ss.max_anisotropy = static_cast<unsigned int>(json["max_anisotropy"].getInteger());
	}

	if (ss.wrap_u == Gleam::ISamplerState::WRAP_BORDER || ss.wrap_v == Gleam::ISamplerState::WRAP_BORDER ||
		ss.wrap_w == Gleam::ISamplerState::WRAP_BORDER) {

		ss.border_r = static_cast<float>(json["border_r"].getReal());
		ss.border_g = static_cast<float>(json["border_g"].getReal());
		ss.border_b = static_cast<float>(json["border_b"].getReal());
		ss.border_a = static_cast<float>(json["border_a"].getReal());
	}

	SamplerStateData* sampler_data = GetAllocator()->template allocT<SamplerStateData>();

	if (!sampler_data) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate Sampler State data structure.\n", file_name);
		return nullptr;
	}

	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	
	sampler_data->data.resize(rd.getNumDevices());

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		SamplerStatePtr sampler(_render_mgr.createSamplerState());
		rd.setCurrentDevice(i);

		if (!sampler) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate Sampler State for sampler '%s'.\n", file_name);
			GetAllocator()->freeT(sampler_data);
			return nullptr;
		}

		if (!sampler->init(rd, ss)) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize Sampler State for sampler '%s'.\n", file_name);
			GetAllocator()->freeT(sampler_data);
			return nullptr;
		}

		sampler_data->data[i] = Gaff::Move(sampler);
	}

	return sampler_data;
}

NS_END
