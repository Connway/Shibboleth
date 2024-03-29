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

#include "Shibboleth_SamplerStateResource.h"
#include "Shibboleth_GraphicsReflection.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_IFileSystem.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SamplerStateResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResExtAttribute(u8".sampler_state.bin"),
		Shibboleth::ResExtAttribute(u8".sampler_state"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::SamplerStateResource::loadSamplerState)
	)

	.base<Shibboleth::IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::SamplerStateResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(SamplerStateResource)

Vector<Gleam::IRenderDevice*> SamplerStateResource::getDevices(void) const
{
	Vector<Gleam::IRenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _sampler_states) {
		out.emplace_back(const_cast<Gleam::IRenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool SamplerStateResource::createSamplerState(const Vector<Gleam::IRenderDevice*>& devices, const Gleam::ISamplerState::Settings& sampler_state_settings)
{
	bool success = true;

	for (Gleam::IRenderDevice* device : devices) {
		success = success && createSamplerState(*device, sampler_state_settings);
	}

	return success;
}

bool SamplerStateResource::createSamplerState(Gleam::IRenderDevice& device, const Gleam::ISamplerState::Settings& sampler_state_settings)
{
	const IRenderManager& render_mgr = GETMANAGERT(Shibboleth::IRenderManager, Shibboleth::RenderManager);
	Gleam::ISamplerState* const sampler_state = render_mgr.createSamplerState();

	if (!sampler_state->init(device, sampler_state_settings)) {
		LogErrorResource("Failed to create sampler state '%s'.", getFilePath().getBuffer());
		SHIB_FREET(sampler_state, GetAllocator());
		return false;
	}

	_sampler_states[&device].reset(sampler_state);
	return true;
}

const Gleam::ISamplerState* SamplerStateResource::getSamplerState(const Gleam::IRenderDevice& rd) const
{
	const auto it = _sampler_states.find(&rd);
	return (it != _sampler_states.end()) ? it->second.get() : nullptr;
}

Gleam::ISamplerState* SamplerStateResource::getSamplerState(const Gleam::IRenderDevice& rd)
{
	const auto it = _sampler_states.find(&rd);
	return (it != _sampler_states.end()) ? it->second.get() : nullptr;
}

void SamplerStateResource::loadSamplerState(IFile* file, uintptr_t /*thread_id_int*/)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load raster state '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const RenderManagerBase& render_mgr = GETMANAGERT(Shibboleth::RenderManagerBase, Shibboleth::RenderManager);
	const ISerializeReader& reader = *readerWrapper.getReader();
	const Vector<Gleam::IRenderDevice*>* devices = nullptr;
	U8String device_tag;

	{
		const auto guard = reader.enterElementGuard(u8"devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed shader '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char8_t* const tag = reader.readString(u8"main");
		device_tag = tag;
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load shader '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer(), device_tag.data());
		failed();
		return;
	}

	Gleam::ISamplerState::Settings sampler_state_settings;

	if (!Refl::Reflection<Gleam::ISamplerState::Settings>::GetInstance().load(reader, sampler_state_settings)) {
		LogErrorResource("Failed to load sampler state '%s'. Failed to deserialize sampler settings.", getFilePath().getBuffer());
		failed();
		return;
	}

	if (createSamplerState(*devices, sampler_state_settings)) {
		succeeded();
	} else {
		failed();
	}
}

NS_END
