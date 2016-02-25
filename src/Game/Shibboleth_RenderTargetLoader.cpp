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

/*
	NOTE: data->width/height calculations are going to be wrong for data with mixed sizes. (eg: -1/-1 and actual with/height values mixing)
*/

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

static bool addOutput(Gleam::IRenderDevice& rd, RenderManager& rm, Gleam::IRenderTarget* rt, RenderTargetData* data, int window_width, int window_height, const Gaff::JSON& settings, const char* file_name)
{
	Gleam::ITexture::FORMAT t_fmt = GetEnumRefDef<Gleam::ITexture::FORMAT>().getValue(settings["Format"].getString());
	int width = static_cast<int>(settings["Width"].getInteger());
	int height = static_cast<int>(settings["Height"].getInteger());

	if (width == -1 || height == -1) {
		// If window_width and window_height are also -1, then we are using a bad tag.
		if (window_width == -1 || window_height == -1) {
			LogMessage(
				GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR,
				"ERROR - Depth-Stencil buffer for Render Target '%s' was marked to use window dimensions, but window tag has more than one window associated with it.\n",
				file_name
			);

			return false;
		}

		width = window_width;
		height = window_height;

		data->width = static_cast<unsigned int>(window_width);
		data->height = static_cast<unsigned int>(window_height);

	} else {
		data->width = Gaff::Max(static_cast<unsigned int>(width), data->width);
		data->height = Gaff::Max(static_cast<unsigned int>(height), data->height);
	}

	TexturePtr texture(rm.createTexture());

	if (!texture) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate texture for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!texture->init2D(rd, width, height, t_fmt)) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!rt->addTexture(rd, texture.get())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	ShaderResourceViewPtr srv(rm.createShaderResourceView());

	if (!srv) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!srv->init(rd, texture.get())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	data->texture_srvs[rd.getCurrentDevice()].emplacePush(std::move(srv));
	data->textures[rd.getCurrentDevice()].emplacePush(std::move(texture));

	return true;
}

static bool addDepthStencil(Gleam::IRenderDevice& rd, RenderManager& rm, Gleam::IRenderTarget* rt, RenderTargetData* data, int window_width, int window_height, const Gaff::JSON& settings, const char* file_name)
{
	// We're not adding Depth-Stencil to this Render Target.
	if (!settings["Depth-Stencil Format"].isString()) {
		return true;
	}

	AString depth_stencil_format(settings["Depth-Stencil Format"].getString());

	Gleam::ITexture::FORMAT ds_fmt = GetEnumRefDef<Gleam::ITexture::FORMAT>().getValue(depth_stencil_format.getBuffer());

	if (ds_fmt < Gleam::ITexture::DEPTH_16_UNORM) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Render Target file '%s' is malformed. Value at 'Depth-Stencil Format' is not a depth-stencil format.\n", file_name);
		return false;
	}

	int width = static_cast<int>(settings["Depth-Stencil Width"].getInteger());
	int height = static_cast<int>(settings["Depth-Stencil Height"].getInteger());

	if (width == -1 || height == -1) {
		// If window_width and window_height are also -1, then we are using a bad tag.
		if (window_width == -1 || window_height == -1) {
			LogMessage(
				GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR,
				"ERROR - Depth-Stencil buffer for Render Target '%s' was marked to use window dimensions, but window tag has more than one window associated with it.\n",
				file_name
			);

			return false;
		}

		width = window_width;
		height = window_height;

		// Use the largest texture in the group for width/height.
		data->width = static_cast<unsigned int>(window_width);
		data->height = static_cast<unsigned int>(window_height);

	} else {
		// Just take the largest texture size as the width/height value for this render target.
		data->width = Gaff::Max(static_cast<unsigned int>(width), data->width);
		data->height = Gaff::Max(static_cast<unsigned int>(height), data->height);
	}

	TexturePtr texture(rm.createTexture());

	if (!texture) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate depth-stencil texture for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!texture->initDepthStencil(rd, width, height, ds_fmt)) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize depth-stencil texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!rt->addDepthStencilBuffer(rd, texture.get())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate depth-stencil texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	ShaderResourceViewPtr srv(rm.createShaderResourceView());

	if (!srv) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate depth-stencil shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!srv->init(rd, texture.get())) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize depth-stencil shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	data->depth_stencil_srvs[rd.getCurrentDevice()] = srv;
	data->depth_stencils[rd.getCurrentDevice()] = texture;

	return true;
}


RenderTargetLoader::RenderTargetLoader(void)
{
}

RenderTargetLoader::~RenderTargetLoader(void)
{
}

Gaff::IVirtualDestructor* RenderTargetLoader::load(const char* file_name, uint64_t, HashMap<AString, IFile*>& file_map)
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
		disp_tags = 0;

		if (EXTRACT_DISPLAY_TAGS(display_tags, disp_tags)) {
			// This should never get hit. Schema check would have failed.
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Render Target file '%s' is malformed. An element in 'display_tags' is not a string.\n", file_name);
			return nullptr;
		}
	}

	// RenderTargetData setup
	RenderTargetData* data = GetAllocator()->template allocT<RenderTargetData>();
	RenderManager& rm = GetApp().getManagerT<RenderManager>("Render Manager");
	Gleam::IRenderDevice& rd = rm.getRenderDevice();

	if (!data) {
		LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate data for Render Target for '%s'.\n", file_name);
		return nullptr;
	}

	data->render_targets.resize(rd.getNumDevices());
	data->textures.resize(rd.getNumDevices());
	data->texture_srvs.resize(rd.getNumDevices());
	data->depth_stencils.resize(rd.getNumDevices());
	data->depth_stencil_srvs.resize(rd.getNumDevices());

	data->tags = disp_tags;
	data->any_display_with_tags = rt_settings["Any Display With Tags"].isTrue();

	const Gaff::JSON& outputs = rt_settings["Outputs"];

	// Get windows with these tags
	Array<const RenderManager::WindowData*> windows = (data->any_display_with_tags) ?
		rm.getWindowsWithTagsAny(disp_tags) :
		rm.getWindowsWithTags(disp_tags);

	assert(!windows.empty());

	// Cache the window width/height if there is only one window with this tag
	int window_width = -1;
	int window_height = -1;

	if (windows.size() == 1) {
		window_width = static_cast<int>(windows[0]->window->getWidth());
		window_height = static_cast<int>(windows[0]->window->getHeight());
	}

	for (auto it = windows.begin(); it != windows.end(); ++it) {
		// Skip if we have already made a render target for this device
		if (data->render_targets[(*it)->device]) {
			continue;
		}

		RenderTargetPtr rt(rm.createRenderTarget());
		rd.setCurrentDevice((*it)->device);

		if (!rt) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to allocate Render Target '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		if (!rt->init()) {
			LogMessage(GetApp().getGameLogFile(), TPT_PRINTLOG, LogManager::LOG_ERROR, "ERROR - Failed to initialize Render Target for '%s'.\n", file_name);
			GetAllocator()->freeT(data);
			return nullptr;
		}

		if (outputs.isArray()) {
			bool failed = outputs.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
			{
				if (!addOutput(rd, rm, rt.get(), data, window_width, window_height, value, file_name)) {
					return true;
				}

				return false;
			});

			if (failed) {
				GetAllocator()->freeT(data);
				return nullptr;
			}
		}

		if (!addDepthStencil(rd, rm, rt.get(), data, window_width, window_height, rt_settings, file_name)) {
			GetAllocator()->freeT(data);
			return nullptr;
		}

		data->render_targets[(*it)->device] = rt;
	}

	return data;
}

NS_END
