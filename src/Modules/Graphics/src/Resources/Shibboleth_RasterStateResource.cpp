/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Resources/Shibboleth_RasterStateResource.h"
#include "Shibboleth_GraphicsReflection.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_IFileSystem.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_RasterState.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RasterStateResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResourceExtensionAttribute(u8".raster_state.bin"),
		Shibboleth::ResourceExtensionAttribute(u8".raster_state")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::RasterStateResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RasterStateResource)

void RasterStateResource::load(const ISerializeReader& reader, uintptr_t /*thread_id_int*/)
{
	const RenderManager& render_mgr = GetManagerTFast<RenderManager>();
	const Vector<Gleam::RenderDevice*>* devices = nullptr;
	U8String device_tag;

	{
		const auto guard = reader.enterElementGuard(u8"devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed raster state '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char8_t* const tag = reader.readString(u8"main");
		device_tag = tag;
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load raster state '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer(), device_tag.data());
		failed();
		return;
	}

	Gleam::IRasterState::Settings raster_state_settings;

	if (!Refl::Reflection<Gleam::IRasterState::Settings>::GetInstance().load(reader, raster_state_settings)) {
		LogErrorResource("Failed to load raster state '%s'. Failed to deserialize raster settings.", getFilePath().getBuffer());
		failed();
		return;
	}

	if (createRasterState(*devices, raster_state_settings)) {
		succeeded();
	} else {
		failed();
	}
}

Vector<Gleam::RenderDevice*> RasterStateResource::getDevices(void) const
{
	Vector<Gleam::RenderDevice*> out{ GRAPHICS_ALLOCATOR };

	for (const auto& pair : _raster_states) {
		out.emplace_back(const_cast<Gleam::RenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool RasterStateResource::createRasterState(const Vector<Gleam::RenderDevice*>& devices, const Gleam::IRasterState::Settings& raster_state_settings)
{
	bool success = true;

	for (Gleam::RenderDevice* device : devices) {
		success = success && createRasterState(*device, raster_state_settings);
	}

	return success;
}

bool RasterStateResource::createRasterState(Gleam::RenderDevice& device, const Gleam::IRasterState::Settings& raster_state_settings)
{
	static ProxyAllocator k_allocator("Graphics");
	Gleam::RasterState* const raster_state = SHIB_ALLOCT(Gleam::RasterState, k_allocator);

	if (!raster_state->init(device, raster_state_settings)) {
		LogErrorResource("Failed to create raster state '%s'.", getFilePath().getBuffer());
		SHIB_FREET(raster_state, k_allocator);

		return false;
	}

	_raster_states[&device].reset(raster_state);
	return true;
}

const Gleam::RasterState* RasterStateResource::getRasterState(const Gleam::RenderDevice& rd) const
{
	const auto it = _raster_states.find(&rd);
	return (it != _raster_states.end()) ? it->second.get() : nullptr;
}

Gleam::RasterState* RasterStateResource::getRasterState(const Gleam::RenderDevice& rd)
{
	const auto it = _raster_states.find(&rd);
	return (it != _raster_states.end()) ? it->second.get() : nullptr;
}

NS_END