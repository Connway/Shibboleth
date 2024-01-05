/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_IFileSystem.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_SamplerState.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SamplerStateResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResourceExtensionAttribute(u8".sampler_state.bin"),
		Shibboleth::ResourceExtensionAttribute(u8".sampler_state")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::SamplerStateResource)

NS_SHIBBOLETH

static ProxyAllocator g_allocator("Graphics");

SHIB_REFLECTION_CLASS_DEFINE(SamplerStateResource)

void SamplerStateResource::load(const ISerializeReader& reader, uintptr_t /*thread_id_int*/)
{
	const RenderManagerBase& render_mgr = GETMANAGERT(Shibboleth::RenderManagerBase, Shibboleth::RenderManager);
	const Vector<Gleam::RenderDevice*>* devices = nullptr;
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

Vector<Gleam::RenderDevice*> SamplerStateResource::getDevices(void) const
{
	Vector<Gleam::RenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _sampler_states) {
		out.emplace_back(const_cast<Gleam::RenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool SamplerStateResource::createSamplerState(const Vector<Gleam::RenderDevice*>& devices, const Gleam::ISamplerState::Settings& sampler_state_settings)
{
	bool success = true;

	for (Gleam::RenderDevice* device : devices) {
		success = success && createSamplerState(*device, sampler_state_settings);
	}

	return success;
}

bool SamplerStateResource::createSamplerState(Gleam::RenderDevice& device, const Gleam::ISamplerState::Settings& sampler_state_settings)
{
	Gleam::SamplerState* const sampler_state = SHIB_ALLOCT(Gleam::SamplerState, g_allocator);

	if (!sampler_state->init(device, sampler_state_settings)) {
		LogErrorResource("Failed to create sampler state '%s'.", getFilePath().getBuffer());
		SHIB_FREET(sampler_state, GetAllocator());
		return false;
	}

	_sampler_states[&device].reset(sampler_state);
	return true;
}

const Gleam::SamplerState* SamplerStateResource::getSamplerState(const Gleam::RenderDevice& rd) const
{
	const auto it = _sampler_states.find(&rd);
	return (it != _sampler_states.end()) ? it->second.get() : nullptr;
}

Gleam::SamplerState* SamplerStateResource::getSamplerState(const Gleam::RenderDevice& rd)
{
	const auto it = _sampler_states.find(&rd);
	return (it != _sampler_states.end()) ? it->second.get() : nullptr;
}

NS_END
