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

#include "Shibboleth_RasterStateResource.h"
#include "Shibboleth_GraphicsReflection.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(RasterStateResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(RasterStateResource)
	.classAttrs(
		CreatableAttribute(),
		ResExtAttribute(".raster_state.bin"),
		ResExtAttribute(".raster_state"),
		MakeLoadFileCallbackAttribute(&RasterStateResource::loadRasterState)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(RasterStateResource)

const Gleam::IRasterState* RasterStateResource::getRasterState(const Gleam::IRenderDevice& rd) const
{
	const auto it = _raster_states.find(&rd);
	return (it != _raster_states.end()) ? it->second.get() : nullptr;
}

Gleam::IRasterState* RasterStateResource::getRasterState(const Gleam::IRenderDevice& rd)
{
	const auto it = _raster_states.find(&rd);
	return (it != _raster_states.end()) ? it->second.get() : nullptr;
}

void RasterStateResource::loadRasterState(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load raster state '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();
	const Vector<Gleam::IRenderDevice*>* devices = nullptr;

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

	Gleam::IRasterState::RasterStateSettings raster_state_settings;
	Reflection<Gleam::IRasterState::RasterStateSettings>::Load(reader, raster_state_settings);

	for (Gleam::IRenderDevice* rd : *devices) {
		Gleam::IRasterState* const raster_state = render_mgr.createRasterState();

		if (!raster_state->init(*rd, raster_state_settings)) {
			LogErrorResource("Failed to create raster state '%s'.", getFilePath().getBuffer());
			failed();

			SHIB_FREET(raster_state, GetAllocator());
			continue;
		}

		_raster_states[rd].reset(raster_state);
	}

	if (!hasFailed()) {
		succeeded();
	}
}

NS_END
