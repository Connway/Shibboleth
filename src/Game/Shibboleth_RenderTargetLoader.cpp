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

#include "Shibboleth_RenderTargetLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_TaskPoolTags.h"
#include <Shibboleth_SchemaManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_ITexture.h>

NS_SHIBBOLETH

RenderTargetLoader::RenderTargetLoader(void)
{
}

RenderTargetLoader::~RenderTargetLoader(void)
{
}

Gaff::IVirtualDestructor* RenderTargetLoader::load(const char* file_name, unsigned long long, HashMap<AString, IFile*>& file_map)
{
	assert(file_map.hasElementWithKey(AString(file_name)));
	IFile*& file = file_map[AString(file_name)];

	Gaff::JSON rt_settings;
	bool success = rt_settings.parse(file->getBuffer());

	GetApp().getFileSystem()->closeFile(file);
	file = nullptr;

	if (!success) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to parse file '%s'.\n", file_name);
		return nullptr;
	}

	static const Gaff::JSON& schema = GetApp().getManagerT<SchemaManager>("Schema Manager").getSchema("RenderTarget.schema");

	if (!rt_settings.validate(schema)) {
		LogMessage(
			GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR,
			"ERROR - Render Target file '%s' has bad schema for reason '%s' (line %i).\n",
			file_name,
			rt_settings.getErrorText(),
			rt_settings.getErrorLine()
		);

		return nullptr;
	}

	Gaff::JSON display_tags = rt_settings["Display Tags"];
	unsigned short disp_tags = DT_ALL;

	if (!display_tags.isNull()) {
		if (!display_tags.isArray()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Render Target file '%s' is malformed. Value at 'display_tags' is not an array of strings.\n", file_name);
			return nullptr;
		}

		disp_tags = 0;

		if (EXTRACT_DISPLAY_TAGS(display_tags, disp_tags)) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Render Target file '%s' is malformed. An element in 'display_tags' is not a string.\n", file_name);
			return nullptr;
		}
	}

	AString depth_stencil_format;

	if (rt_settings["Depth-Stencil Format"].isString()) {
		depth_stencil_format = rt_settings["Depth-Stencil Format"].getString();
	}

	Gleam::ITexture::FORMAT ds_fmt = Gleam::ITexture::R_8_UNORM;

	if (depth_stencil_format.size()) {
		ds_fmt = GetEnumRefDef<Gleam::ITexture::FORMAT>().getValue(depth_stencil_format.getBuffer());

		if (ds_fmt < Gleam::ITexture::DEPTH_16_UNORM) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Render Target file '%s' is malformed. Value at 'Depth-Stencil Format' is not a depth-stencil format.\n", file_name);
			return nullptr;
		}
	}

	RenderTargetData* data = GetAllocator()->template allocT<RenderTargetData>();

	if (!data) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate data for Render Target for '%s'.\n", file_name);
		return nullptr;
	}

	data->tags = disp_tags;

	RenderManager& rm = GetApp().getManagerT<RenderManager>("Render Manager");
	Gleam::IRenderDevice& rd = rm.getRenderDevice();

	data->render_targets.resize(rd.getNumDevices());


	if (depth_stencil_format.size()) {
		data->depth_stencils.resize(rd.getNumDevices());
		data->depth_stencil_srvs.resize(rd.getNumDevices());
	}

	Array<const RenderManager::WindowData*> windows = (rt_settings["Any Display With Tags"].isTrue()) ?
		rm.getWindowsWithTagsAny(disp_tags) :
		rm.getWindowsWithTags(disp_tags);

	assert(!windows.empty());

	int ds_width = static_cast<int>(rt_settings["Depth-Stencil Width"].getInteger());
	int ds_height = static_cast<int>(rt_settings["Depth-Stencil Height"].getInteger());

	if (ds_width == -1 || ds_height == -1) {
		if (windows.size() != 1) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Width/Height set to window size, but window tags returned more than one window for render target '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		ds_width = static_cast<int>(windows[0]->window->getWidth());
		ds_height = static_cast<int>(windows[0]->window->getHeight());
	}

	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (data->render_targets[(*it)->device]) {
			continue;
		}

		RenderTargetPtr rt(rm.createRenderTarget());
		rd.setCurrentDevice((*it)->device);

		if (!rt) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate render target '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		if (!rt->init()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize render target for '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		const Gaff::JSON& outputs = rt_settings["Outputs"];

		if (outputs.isArray()) {
			bool failed = outputs.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
			{
				if (!addOutput(rd, rt.get(), value)) {
					return true;
				}

				return false;
			});

			if (failed) {
				return nullptr;
			}
		}

		if (depth_stencil_format.size()) {
			TexturePtr texture(rm.createTexture());

			if (texture) {
				if (!texture->initDepthStencil(rd, ds_width, ds_height, ds_fmt)) {
					LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize depth-stencil texture data for Render Target '%s'.\n", file_name);
					GetAllocator()->freeT(data);
					return nullptr;
				}
			}

			if (!texture || !rt->addDepthStencilBuffer(rd, texture.get())) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate depth-stencil texture data for Render Target '%s'.\n", file_name);
				GetAllocator()->freeT(data);
				return nullptr;
			}

			ShaderResourceViewPtr srv(rm.createShaderResourceView());

			if (!srv || !srv->init(rd, texture.get())) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to create shader resource view for Render Target '%s'.\n", file_name);
				GetAllocator()->freeT(data);
				return nullptr;
			}

			data->depth_stencil_srvs[(*it)->device] = srv;
			data->depth_stencils[(*it)->device] = texture;
		}

		data->render_targets[(*it)->device] = rt;
	}

	return data;
}

bool RenderTargetLoader::addOutput(Gleam::IRenderDevice& rd, Gleam::IRenderTarget* rt, const Gaff::JSON& settings)
{
	int width = static_cast<int>(settings["Width"].getInteger());
	int height = static_cast<int>(settings["Height"].getInteger());
	AString texture_format(settings["Format"].getString());

	Gleam::ITexture::FORMAT t_fmt = Gleam::ITexture::R_8_UNORM;

	if (texture_format.size()) {
		t_fmt = GetEnumRefDef<Gleam::ITexture::FORMAT>().getValue(texture_format.getBuffer());
	}

	if (texture_format.size()) {
		data->textures.resize(rd.getNumDevices());
		data->texture_srvs.resize(rd.getNumDevices());
	}

	if (texture_format.size()) {
		TexturePtr texture(rm.createTexture());

		if (texture) {
			if (!texture->init2D(rd, width, height, t_fmt)) {
				LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize texture data for Render Target '%s'.\n", file_name);
				GetAllocator()->freeT(data);
				return nullptr;
			}
		}

		if (!texture || !rt->addTexture(rd, texture.get())) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate texture data for Render Target '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		ShaderResourceViewPtr srv(rm.createShaderResourceView());

		if (!srv || !srv->init(rd, texture.get())) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to create shader resource view for Render Target '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		data->texture_srvs[(*it)->device] = srv;
		data->textures[(*it)->device] = texture;
	}

	return true;
}

bool RenderTargetLoader::addDepthStencil(Gleam::IRenderDevice& rd, Gleam::IRenderTarget* rt, const Gaff::JSON& settings)
{
	return true;
}

NS_END
